
#include <grabber/AudioGrabberWindows.h>
#include <grabber/AudioGrabber.h>
#include <windows.media.audio.h>
#include <QImage>

AudioGrabberWindows::AudioGrabberWindows(const QString& device) : AudioGrabber(device)
{
	// init
	
}

bool AudioGrabberWindows::startAudio()
{
	return true;
}

void AudioGrabberWindows::stopAudio()
{

}

QObjectList listAudioDevices()
{

}

int16_t AudioGrabberWindows::grabAudioFrame(int16_t* buffer)
{
	return 0;
}

void AudioGrabberWindows::freeResources()
{
	this->stopAudio();
}
