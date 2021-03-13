#include <grabber/AudioGrabberLinux.h>
#include <grabber/AudioGrabber.h>
#include <alsa/asoundlib.h>
#include <QObject>

typedef void* (*THREADFUNCPTR)(void*);

AudioGrabberLinux::AudioGrabberLinux(const QString& device, const QJsonObject& config) : AudioGrabber(device, config)
{
	// init
	this->refreshDevices();
}

AudioGrabberLinux::~AudioGrabberLinux()
{
	this->freeResources();
}

void AudioGrabberLinux::refreshDevices()
{
	Debug(_log, "Enumerating Audio Input Devices");

	_deviceProperties.clear();

	snd_ctl_t* deviceHandle;
	int soundCard = -1, error = -1, cardInput = -1;

	snd_ctl_card_info_t* cardInfo;
	snd_pcm_info_t* deviceInfo;

	snd_ctl_card_info_alloca(&cardInfo);
	snd_pcm_info_alloca(&deviceInfo);

	while (snd_card_next(&soundCard) > -1)
	{
		if (soundCard < 0)
			break;

		char cardId[32];
		sprintf(cardId, "hw:%d", soundCard);

		if ((error = snd_ctl_open(&deviceHandle, cardId, 0)) < 0)
		{
			Error(_log, "Erorr opening device: (%i): %s", soundCard, snd_strerror(error));
			continue;
		}

		if ((error = snd_ctl_card_info(deviceHandle, cardInfo)) < 0)
		{
			Error(_log, "Erorr getting hardware info: (%i): %s", soundCard, snd_strerror(error));
			snd_ctl_close(deviceHandle);
			continue;
		}

		cardInput = -1;

		while (true)
		{
			if (snd_ctl_pcm_next_device(deviceHandle, &cardInput) < 0)
				Error(_log, "Error selecting device input");

			if (cardInput < 0)
				break;

			snd_pcm_info_set_device(deviceInfo, cardInput);
			snd_pcm_info_set_subdevice(deviceInfo, 0);
			snd_pcm_info_set_stream(deviceInfo, SND_PCM_STREAM_CAPTURE);

			if ((error = snd_ctl_pcm_info(deviceHandle, deviceInfo)) < 0)
			{
				if (error != -ENOENT)
					Error(_log, "Digital Audio Info: (%i): %s", soundCard, snd_strerror(error));

				continue;
			}

			AudioGrabber::DeviceProperties device;

			char deviceId[32];
			sprintf(deviceId, "hw:%d,%d", snd_pcm_info_get_card(deviceInfo), snd_pcm_info_get_device(deviceInfo));

			QString absoluteDeviceId	= QString(deviceId);

			char deviceDesc[164];
			sprintf(deviceDesc, "%s: %s", snd_ctl_card_info_get_name(cardInfo), snd_pcm_info_get_name(deviceInfo));

			device.name = QString(deviceDesc);

			Debug(_log, "Found card (%s): %s", deviceId, device.name.toStdString().c_str());

			_deviceProperties.insert(absoluteDeviceId, device);
		}

		snd_ctl_close(deviceHandle);
	}
}

bool AudioGrabberLinux::configureCaptureInterface()
{
	int error = -1;
	if ((error = snd_pcm_open(&this->captureDevice, (this->_device.isEmpty() || this->_device == "auto") ? "default" : this->_device.toStdString().c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0)
	{
		Error(_log, "Failed to open audio device: %s, - %s", this->_device.toStdString().c_str(), snd_strerror(error));
		return false;
	}

	if ((error = snd_pcm_hw_params_malloc(&this->captureDeviceConfig)) < 0)
	{
		Error(_log, "Failed to create hardware parameters: %s", snd_strerror(error));
		snd_pcm_close(this->captureDevice);
		return false;
	}

	if ((error = snd_pcm_hw_params_any(this->captureDevice, this->captureDeviceConfig)) < 0)
	{
		Error(_log, "Failed to initialize hardware parameters: %s", snd_strerror(error));
		snd_pcm_hw_params_free(this->captureDeviceConfig);
		snd_pcm_close(this->captureDevice);
		return false;
	}
	
	if ((error = snd_pcm_hw_params_set_access(this->captureDevice, this->captureDeviceConfig, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
	{
		Error(_log, "Failed to configure non-interleaved mode: %s", snd_strerror(error));
		snd_pcm_hw_params_free(this->captureDeviceConfig);
		snd_pcm_close(this->captureDevice);
		return false;
	}
	
	if ((error = snd_pcm_hw_params_set_format(this->captureDevice, this->captureDeviceConfig, SND_PCM_FORMAT_S16_LE)) < 0)
	{
		Error(_log, "Failed to configure capture format: %s", snd_strerror(error));
		snd_pcm_hw_params_free(this->captureDeviceConfig);
		snd_pcm_close(this->captureDevice);
		return false;
	}

	if ((error = snd_pcm_hw_params_set_rate_near(this->captureDevice, this->captureDeviceConfig, &this->sampleRate, 0)) < 0)
	{
		Error(_log, "Failed to configure sample rate: %s", snd_strerror(error));
		snd_pcm_hw_params_free(this->captureDeviceConfig);
		snd_pcm_close(this->captureDevice);
		return false;
	}

	/* -- Fails for some audio devices
	if ((error = snd_pcm_hw_params_set_channels(this->captureDevice, this->captureDeviceConfig, 2)) < 0)
	{
		Error(_log, "Failed to configure single channel capture: %s", snd_strerror(error));
		snd_pcm_hw_params_free(this->captureDeviceConfig);
		snd_pcm_close(this->captureDevice);
		return false;
	}
	*/

	if ((error = snd_pcm_hw_params(this->captureDevice, this->captureDeviceConfig)) < 0)
	{
		Error(_log, "Failed to configure hardware parameters: %s", snd_strerror(error));
		snd_pcm_hw_params_free(this->captureDeviceConfig);
		snd_pcm_close(this->captureDevice);
		return false;
	}

	snd_pcm_hw_params_free(this->captureDeviceConfig);

	if ((error = snd_pcm_nonblock(this->captureDevice, 1)) < 0)
	{
		Error(_log, "Failed to configure non-blocking mode: %s", snd_strerror(error));
		snd_pcm_close(this->captureDevice);
		return false;
	}
	
	if ((error = snd_pcm_prepare(this->captureDevice)) < 0)
	{
		Error(_log, "Failed to prepare audio interface: %s", snd_strerror(error));
		snd_pcm_close(this->captureDevice);
		return false;
	}

	if ((error = snd_pcm_start(this->captureDevice)) < 0)
	{
		Error(_log, "Failed to start audio interface: %s", snd_strerror(error));
		snd_pcm_close(this->captureDevice);
		return false;
	}
	
	return true;
}

bool AudioGrabberLinux::startAudio()
{
	if (!_enabled)
		return false;

	AudioGrabber::startAudio();

	Debug(_log, "Start Audio With %s", this->getDeviceName(this->_device).toStdString().c_str());
	

	if (!this->configureCaptureInterface())
		return false;

	this->isRunning.store(true, std::memory_order_release);

	if (pthread_create(&this->audioThread, NULL, (THREADFUNCPTR) &AudioThreadRunner, (void*) this) != 0)
	{
		Debug(_log, "Failed to create audio capture thread");
		this->stopAudio();
		return false;
	}

	return true;
}

void AudioGrabberLinux::stopAudio()
{
	if (!this->isRunning.load(std::memory_order_acquire))
		return;

	AudioGrabber::stopAudio();

	Debug(_log, "Stopping Audio Interface");

	this->isRunning.store(false, std::memory_order_release);

	if (this->audioThread != 0)
		pthread_join(this->audioThread, NULL);

	snd_pcm_close(this->captureDevice);
}

void AudioGrabberLinux::processAudioBuffer(int frames)
{
	if (!this->isRunning.load(std::memory_order_acquire))
		return;

	ssize_t bytes = snd_pcm_frames_to_bytes(this->captureDevice, frames);

	int16_t * buffer = (int16_t*)calloc(bytes / 2, sizeof(int16_t)); // * snd_pcm_format_width(SND_PCM_FORMAT_S16_LE) / 8 * 2);
	
	if (frames == 0)
	{
		buffer[0] = 0;
		this->processAudioFrame(buffer, 1);
	}
	else
	{
		int framesRead = snd_pcm_readi(this->captureDevice, buffer, frames);

		if (framesRead < frames)
		{
			Error(_log, "Error reading audio. Got %d frames instead of %d", framesRead, frames);
		}
		else
			this->processAudioFrame(buffer, snd_pcm_frames_to_bytes(this->captureDevice, framesRead) / 2);
	}

	free(buffer);
}

QStringList AudioGrabberLinux::getDevices() const
{
	QStringList result = QStringList();
	for (auto it = _deviceProperties.begin(); it != _deviceProperties.end(); ++it)
	{
		result << it.key();
	}

	return result;
}

QString AudioGrabberLinux::getDeviceName(const QString& devicePath) const
{
	if (devicePath.isEmpty() || devicePath == "auto")
		return "Default Device";

	return _deviceProperties.value(devicePath).name;
}

QMultiMap<QString, int> AudioGrabberLinux::getDeviceInputs(const QString& devicePath) const
{
	return _deviceProperties.value(devicePath).inputs;
}

void AudioGrabberLinux::freeResources()
{
	this->stopAudio();
}


static void * AudioThreadRunner(void* params)
{
	AudioGrabberLinux* This = (AudioGrabberLinux*)params;

	Debug(This->getLog(), "Audio Thread Started");

	int framesAvailable = 0;

			
	while (This->isRunning.load(std::memory_order_acquire))
	{
		snd_pcm_wait(This->captureDevice, 1000);

		if ((framesAvailable = snd_pcm_avail(This->captureDevice)) > 0)
			This->processAudioBuffer(framesAvailable);

		pthread_yield();
	}

	Debug(This->getLog(), "Audio Thread Shutting Down");

	return 0;
}
