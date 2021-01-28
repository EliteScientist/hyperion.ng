#include <grabber/AudioGrabber.h>
#include <QImage>

AudioGrabber::AudioGrabber(const QString& device)
	: Grabber("AudioGrabber")
{
	// init
}

AudioGrabber::~AudioGrabber()
{
	freeResources();
}

void AudioGrabber::freeResources()
{
	this->stopAudio();
}

int AudioGrabber::grabFrame(Image<ColorRgb>& image)
{
	if (!_enabled)
		return 0;

	// Define buffer

	// Grab audio into buffer

	// Process visualizer

	// Fill image

	// Free buffer
	

	return 0;
}
