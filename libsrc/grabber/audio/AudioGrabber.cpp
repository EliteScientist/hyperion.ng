#include <grabber/AudioGrabber.h>
#include <math.h>
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

void AudioGrabber::processAudioFrame(int16_t* buffer, int length)
{
	// Apply Visualizer and Construct Image

	// Default UVMeter - Later Make this pluggable for different audio effects
	int32_t bufferSum = 0;

	const uint8_t max = 100;
	const QColor  hotColor = QColor(0x00, 0x00, 0xFF); // For some unknown reason this is stored in a BGR format
	const uint8_t warn = 80;
	const QColor  warnColor = QColor(0x00, 0xFF, 0xFF);
	const uint8_t safe = 45;
	const QColor  safeColor = QColor(0x00, 0xFF, 0x00);
	const uint16_t multiplier = 5;
	
	// Default Empty Color
	const QColor blackColor = QColor(0, 0, 0);

	// Calculate the the average value
	for (int i = 0; i < length; i++)
		bufferSum += abs(buffer[i]);

	const float result = ceil(bufferSum / length) * multiplier;

	// Calculate the average percentage
	const float percentage = result / INT16_MAX;

	// Calculate the value
	const uint8_t value = ceil(percentage * max);

	Debug(_log, "AUDIO VALUE: %u", value);

	// Draw Image
	QImage image(1, max, QImage::Format_RGB888);
	image.fill(blackColor);
	
	for (int i = 0; i < max; i++)
	{
		QColor color = blackColor;
		uint8_t position = max - i;

		if (position < safe)
			color = safeColor;
		else if (position < warn)
			color = warnColor;
		else
			color = hotColor;
		
		if (position < value)
			image.setPixelColor(0, i, color);
		else
			image.setPixelColor(0, i, blackColor);
	}

	// Convert to Image<ColorRGB>
	Image<ColorRgb> finalImage (image.width(), image.height());
	QByteArray imageData;

	imageData.reserve(image.width() * image.height() * 3);

	for (int i = 0; i < image.height(); ++i)
	{
		const QRgb* scanline = reinterpret_cast<const QRgb*>(image.scanLine(i));

		for (int j = 0; j < image.width(); ++j)
		{
			imageData.append((char)qRed(scanline[j]));
			imageData.append((char)qGreen(scanline[j]));
			imageData.append((char)qBlue(scanline[j]));
		}
	}
		
	memcpy(finalImage.memptr(), imageData.data(), imageData.size());

	emit newFrame(finalImage);
}

bool AudioGrabber::startAudio() { return false; }
void AudioGrabber::stopAudio() {}

