#include <grabber/AudioGrabber.h>
#include <math.h>

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

void AudioGrabber::processAudioFrame(int16_t* buffer, int length)
{
	// Apply Visualizer and Construct Image

	// Default UVMeter - Later Make this pluggable for different audio effects
	int32_t bufferSum = 0;

	for (int i = 0; i < length; i++)
		bufferSum += abs(buffer[i]);

	uint16_t result = ceil(bufferSum / length);

	float percentage = result / sizeof(uint16_t);

	int max		= 100;
	int hotColor = 0xFF0000;
	int warn	= 45;
	int warnColor = 0xFFFF00;
	int safeColor = 0x0000FF;

	uint16_t value = ceil(percentage * max);

	Image<ColorRgb> image(1, max);

	for (int i = 0; i < max; i++)
	{
		
	}
	
	emit newFrame(image);

	Debug(_log, "Got Audio Frame Average: %f - %u", percentage, value);
}

bool AudioGrabber::startAudio() { return false; }
void AudioGrabber::stopAudio() {}

