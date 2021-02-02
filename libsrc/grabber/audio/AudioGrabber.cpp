#include <grabber/AudioGrabber.h>
#include <QImage>

AudioGrabber::AudioGrabber(const QString& device)
	: Grabber("AudioGrabber"),
	_log(Logger::getInstance("AudioGrabber")),
	_device(device)
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

void processAudioFrame(int16_t* buffer, int16_t length)
{
	// Apply Visualizer and Construct Image

	// emit newFrame(image);
}

bool AudioGrabber::startAudio() { return false; }
void AudioGrabber::stopAudio() {}

