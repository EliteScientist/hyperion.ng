#include <grabber/AudioGrabberLinux.h>
#include <grabber/AudioGrabber.h>
#include <QImage>


AudioGrabberLinux::AudioGrabberLinux(const QString& device) : AudioGrabber(device)
{
	// init
	this->refreshDevices();
}

void AudioGrabberLinux::refreshDevices()
{
	_deviceProperties.clear();
}

bool AudioGrabberWindows::configureCaptureInterface()
{
	return true;
}

bool AudioGrabberLinux::startAudio()
{
	Debug(_log, "Start Audio With %s", this->getDeviceName(_device).toStdString().c_str());

	

	return true;
}

void AudioGrabberLinux::stopAudio()
{
	if (!this->isRunning.load(std::memory_order_acquire))
		return;

	this->isRunning.store(false, std::memory_order_release);

	
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
	if (devicePath.isEmpty())
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
