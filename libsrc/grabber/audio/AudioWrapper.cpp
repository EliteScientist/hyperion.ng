#include <grabber/AudioWrapper.h>
#include <hyperion/GrabberWrapper.h>
#include <QTimer>
#include <QMetaType>

AudioWrapper::AudioWrapper(const QString & device, const unsigned updateRate_Hz)
	: GrabberWrapper("AudioGrabber", &_grabber, 0, 0, updateRate_Hz)
	, _grabber(device)	
{
	connect(&_grabber, &AudioGrabber::newFrame, this, &AudioWrapper::newFrame, Qt::DirectConnection);
}

void AudioWrapper::action()
{
	transferFrame(_grabber);
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
