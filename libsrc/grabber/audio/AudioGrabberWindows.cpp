
#include <grabber/AudioGrabberWindows.h>
#include <grabber/AudioGrabber.h>
#include <QImage>

#pragma comment(lib,"dsound.lib")
#pragma comment(lib, "dxguid.lib")

AudioGrabberWindows::AudioGrabberWindows(const QString& device) : AudioGrabber(device)
{
	// init
	this->refreshDevices();
}

void AudioGrabberWindows::refreshDevices()
{
	_deviceProperties.clear();

	if (FAILED(DirectSoundCaptureEnumerate(DirectSoundEnumProcessor, (VOID*) &_deviceProperties)))
	{
		// TODO: Handle Failure
		Error(_log, "Failed to enumerate audio devices.");
	}
}

bool AudioGrabberWindows::configureCaptureInterface()
{
	LPCLSID deviceId;

	if (this->_device.isEmpty())
	{
		deviceId = NULL;
	}
	else
	{
		wchar_t * deviceIdChar = new wchar_t[_device.length() + 1];
		
		_device.toWCharArray(deviceIdChar);
		deviceIdChar[_device.length()] = 0;

		CLSIDFromString(deviceIdChar, deviceId);

		delete[] deviceIdChar;
	}

	// Create Capture Device
	if (FAILED(DirectSoundCaptureCreate8(deviceId, &recordingDevice, NULL)))
	{
		Error(_log, "Failed to create capture device: %s", this->getDeviceName(_device).toStdString().c_str());
		return false;
	}

	// Define Audio Format & Create Buffer
	WAVEFORMATEX audioFormat = { WAVE_FORMAT_PCM, 1, 44100, 88200, 2, 16, 0 };
	// wFormatTag, nChannels, nSamplesPerSec, mAvgBytesPerSec,
	// nBlockAlign, wBitsPerSample, cbSize
	

	notificationSize = max(1024, audioFormat.nAvgBytesPerSec / 8);
	notificationSize -= notificationSize % audioFormat.nBlockAlign;

	bufferCaptureSize = notificationSize * AUDIO_NOTIFICATION_COUNT;
			
	DSCBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSCBUFFERDESC);
	bufferDesc.dwFlags = 0;
	bufferDesc.dwBufferBytes = bufferCaptureSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &audioFormat;
	bufferDesc.dwFXCount = 0;
	bufferDesc.lpDSCFXDesc = NULL;
				
	// Create Capture Device's Buffer
	LPDIRECTSOUNDCAPTUREBUFFER preBuffer;
	if (FAILED(recordingDevice->CreateCaptureBuffer(&bufferDesc, &preBuffer, NULL)))
	{
		Error(_log, "Failed to create capture buffer: %s", this->getDeviceName(_device).toStdString().c_str());
		recordingDevice->Release();
		return false;
	}

	bufferCapturePosition = 0;

	if (FAILED(preBuffer->QueryInterface(IID_IDirectSoundCaptureBuffer8, (LPVOID*)&recordingBuffer)))
	{
		Error(_log, "Failed to retrieve recording buffer");
		preBuffer->Release();
		return false;
	}

	preBuffer->Release();
		
	// Create Notifications
	LPDIRECTSOUNDNOTIFY8 notify;

	if (FAILED(recordingBuffer->QueryInterface(IID_IDirectSoundNotify8, (LPVOID *) &notify)))
	{
		Error(_log, "Failed to configure buffer notifications: %s", this->getDeviceName(_device).toStdString().c_str());
		recordingDevice->Release();
		recordingBuffer->Release();
		return false;
	}
				
	// Create Events
	notificationEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (notificationEvent == NULL)
	{
		Error(_log, "Failed to configure buffer notifications events: %s", this->getDeviceName(_device).toStdString().c_str());
		notify->Release();
		recordingDevice->Release();
		recordingBuffer->Release();
		return false;
	}

	// Configure Notifications
	DSBPOSITIONNOTIFY positionNotify[AUDIO_NOTIFICATION_COUNT];

	for (int i = 0; i < AUDIO_NOTIFICATION_COUNT; i++)
	{
		positionNotify[i].dwOffset = (notificationSize * i) + notificationSize - 1;
		positionNotify[i].hEventNotify = notificationEvent;
	}
	
	// Set Notifications
	notify->SetNotificationPositions(AUDIO_NOTIFICATION_COUNT, positionNotify);
	notify->Release();
		
	return true;
}

bool AudioGrabberWindows::startAudio()
{
	Debug(_log, "Start Audio With %s", this->getDeviceName(_device).toStdString().c_str());

	if (!this->configureCaptureInterface())
		return false;
		
	if (FAILED(recordingBuffer->Start(DSCBSTART_LOOPING)))
	{
		Error(_log, "Audio capture failed to start: %s", this->getDeviceName(_device).toStdString().c_str());
	}

	this->isRunning.store(true, std::memory_order_release);

	this->audioThread	= CreateThread(NULL, 0, AudioThread, (void *) this, 0, NULL);

	return true;
}

void AudioGrabberWindows::stopAudio()
{
	if (!this->isRunning.load(std::memory_order_acquire))
		return;

	this->isRunning.store(false, std::memory_order_release);

	Debug(_log, "Shutting down audio for: '%s'", this->getDeviceName(_device).toStdString().c_str());

	if (FAILED(recordingBuffer->Stop()))
	{
		Error(_log, "Audio capture failed to stop: '%s'", this->getDeviceName(_device).toStdString().c_str());
	}
		
	if (FAILED(recordingBuffer->Release()))
	{
		Error(_log, "Failed to release recording buffer: '%s'", this->getDeviceName(_device).toStdString().c_str());
	}

	if (FAILED(recordingDevice->Release()))
	{
		Error(_log, "Failed to release recording device: '%s'", this->getDeviceName(_device).toStdString().c_str());
	}

	CloseHandle(notificationEvent);
}

DWORD WINAPI AudioGrabberWindows::AudioThread(LPVOID param)
{
	AudioGrabberWindows* This = (AudioGrabberWindows*)param;

	while (This->isRunning.load(std::memory_order_acquire))
	{
		DWORD result = WaitForMultipleObjects(1, &This->notificationEvent, true, 500);

		switch (result)
		{
			case WAIT_OBJECT_0:
				This->processAudioBuffer();
				break;
		}
	}

	return 0;
}

void AudioGrabberWindows::processAudioBuffer()
{
	DWORD readPosition;
	DWORD capturePosition;

	// Primary segment
	VOID* capturedAudio;
	DWORD capturedAudioLength;

	// Wrap around segment
	VOID* capturedAudio2;
	DWORD capturedAudio2Length;

	LONG lockSize;

	if (FAILED(recordingBuffer->GetCurrentPosition(&capturePosition, &readPosition)))
	{
		// Failed to get current position
		Error(_log, "Failed to get buffer position.");
		return;
	}

	lockSize = readPosition - bufferCapturePosition;

	if (lockSize < 0)
		lockSize += bufferCaptureSize;

	// Block Align
	lockSize -= (lockSize % notificationSize);

	if (lockSize == 0)
		return;

	// Lock Capture Buffer
	if (FAILED(recordingBuffer->Lock(bufferCapturePosition, lockSize, &capturedAudio, &capturedAudioLength,
		&capturedAudio2, &capturedAudio2Length, 0)))
	{
		// Handle Lock Error
		return;
	}

	bufferCapturePosition += capturedAudioLength;
	bufferCapturePosition %= bufferCaptureSize; // Circular Buffer

	int frameSize = capturedAudioLength + capturedAudio2Length;

	int16_t * readBuffer = new int16_t[frameSize];

	// Buffer wrapped around, read second position
	if (capturedAudio2 != NULL)
	{		
		bufferCapturePosition += capturedAudio2Length;
		bufferCapturePosition %= bufferCaptureSize; // Circular Buffer
	}

	// Copy Buffer into memory
	CopyMemory(readBuffer, capturedAudio, capturedAudioLength);

	if (capturedAudio2 != NULL)
		CopyMemory(readBuffer + capturedAudioLength, capturedAudio2, capturedAudio2Length);
			
	// Release Buffer Lock
	recordingBuffer->Unlock(capturedAudio, capturedAudioLength, capturedAudio2, capturedAudio2Length);
	
	// Process Audio Frame
	this->processAudioFrame(readBuffer, frameSize);

	delete[] readBuffer;
}

QStringList AudioGrabberWindows::getDevices() const
{
	QStringList result = QStringList();
	for (auto it = _deviceProperties.begin(); it != _deviceProperties.end(); ++it)
	{
		result << it.key();
	}

	return result;
}

QString AudioGrabberWindows::getDeviceName(const QString& devicePath) const
{
	if (devicePath.isEmpty())
		return "Default Device";

	return _deviceProperties.value(devicePath).name;
}

QMultiMap<QString, int> AudioGrabberWindows::getDeviceInputs(const QString& devicePath) const
{
	return _deviceProperties.value(devicePath).inputs;
}

void AudioGrabberWindows::freeResources()
{
	this->stopAudio();
}
