#include <grabber/AudioWrapper.h>
#include <hyperion/GrabberWrapper.h>
#include <QObject>
#include <QMetaType>

AudioWrapper::AudioWrapper(const QString & device, const QJsonObject& config)
	: GrabberWrapper("AudioGrabber", &_grabber, 0, 0, 10)
	, _grabber(device, config)
{
	// register the image type
	qRegisterMetaType<Image<ColorRgb>>("Image<ColorRgb>");

	connect(&_grabber, &AudioGrabber::newFrame, this, &AudioWrapper::newFrame, Qt::DirectConnection);
}

AudioWrapper::~AudioWrapper()
{
	stop();
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
				
		_grabber.setDevicePath(obj["device"].toString());
		_grabber.setConfiguration(obj);
	}
}
