#include <grabber/AudioWrapper.h>
#include <hyperion/GrabberWrapper.h>
#include <QObject>

AudioWrapper::AudioWrapper(const QString & device, const QJsonObject& config)
	: GrabberWrapper("AudioGrabber", &_grabber, 0, 0, 250)
	, _grabber(device, config)
{
	connect(&_grabber, &AudioGrabber::newFrame, this, &AudioWrapper::newFrame, Qt::DirectConnection);
}

bool AudioWrapper::start()
{
	return (_grabber.startAudio() && GrabberWrapper::start());
}

void AudioWrapper::stop()
{
	_grabber.stopAudio();
	GrabberWrapper::stop();
}

void AudioWrapper::action()
{
	// Dummy we will push the audio images
}

void AudioWrapper::newFrame(const Image<ColorRgb>& image)
{
	emit systemImage(_grabberName, image);
}

void AudioWrapper::handleSettingsUpdate(settings::type type, const QJsonDocument& config)
{
	if (type == settings::AUDIO)
	{
		const QJsonObject& obj = config.object();

		this->stop();
				
		_grabber.setDevicePath(obj["device"].toString());
		_grabber.setConfiguration(obj);

		this->start();
	}
}
