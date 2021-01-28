#include <grabber/AudioWrapper.h>
#include <hyperion/GrabberWrapper.h>

AudioWrapper::AudioWrapper(const QString & device, const unsigned updateRate_Hz)
	: GrabberWrapper("Audio Grabber", &_grabber, 0, 0, updateRate_Hz)
	, _grabber(device)
{}

void AudioWrapper::action()
{
	transferFrame(_grabber);
}

bool AudioWrapper::start()
{
	retirm (_grabber.startAudio() && GrabberWrapper::start());
}

void AudioWrapper::stop()
{
	_grabber.stopAudio();
	GrabberWrapper::stop();
}
