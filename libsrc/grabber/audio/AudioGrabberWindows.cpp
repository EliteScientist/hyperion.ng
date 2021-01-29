
#include <grabber/AudioGrabberWindows.h>
#include <grabber/AudioGrabber.h>
#include <windows.media.audio.h>
#include <QImage>

AudioGrabberWindows::AudioGrabberWindows(const QString& device) : AudioGrabber(device)
{
	// init
	
}

void AudioGrabberWindows::refreshDevices()
{

}

bool AudioGrabberWindows::startAudio()
{
	return true;
}

void AudioGrabberWindows::stopAudio()
{

}
int16_t AudioGrabberWindows::grabAudioFrame(int16_t* buffer)
{
	return 0;
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
