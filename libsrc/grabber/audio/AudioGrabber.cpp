#include <grabber/AudioGrabber.h>
#include <math.h>
#include <QImage>
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QColor>

AudioGrabber::AudioGrabber(const QString& device, const QJsonObject& config)
	: Grabber("AudioGrabber"),
	_log(Logger::getInstance("AudioGrabber")),
	_device(device)
{
	// init
	this->setConfiguration(config);
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

	return 0;
}

void AudioGrabber::setDevicePath(const QString& device)
{
	this->_device	= device;

	if (this->started)
	{
		this->stopAudio();
		this->startAudio();
	}
}

void AudioGrabber::setConfiguration(const QJsonObject& config)
{
	QJsonArray hotColorArray = config["hotColor"].toArray(QJsonArray::fromVariantList(QVariantList::fromStdList({ QVariant(255), QVariant(0), QVariant(0) })));
	QJsonArray warnColorArray = config["warnColor"].toArray(QJsonArray::fromVariantList(QVariantList::fromStdList({ QVariant(255), QVariant(255), QVariant(0) })));
	QJsonArray safeColorArray = config["safeColor"].toArray(QJsonArray::fromVariantList(QVariantList::fromStdList({ QVariant(0), QVariant(255), QVariant(0) })));

	
	this->hotColor = QColor(hotColorArray.at(0).toInt(), hotColorArray.at(1).toInt(), hotColorArray.at(2).toInt());
	this->warnColor = QColor(warnColorArray.at(0).toInt(), warnColorArray.at(1).toInt(), warnColorArray.at(2).toInt());
	this->safeColor = QColor(safeColorArray.at(0).toInt(), safeColorArray.at(1).toInt(), safeColorArray.at(2).toInt());

	this->warnValue = config["warnValue"].toInt(80);
	this->safeValue = config["safeValue"].toInt(45);
	this->multiplier = config["multiplier"].toInt(1);
}

void AudioGrabber::processAudioFrame(int16_t* buffer, int length)
{
	// Apply Visualizer and Construct Image
	
	// TODO: Pass Audio Frame to python and let the script calculate the image.

	// Default UVMeter - Later Make this pluggable for different audio effects
	int32_t bufferSum = 0;
	
	// Calculate the the average value
	for (int i = 0; i < length; i++)
		bufferSum += abs(buffer[i]);

	const float result = ceil(bufferSum / length) * multiplier;

	// Calculate the average percentage
	const float percentage = result / INT16_MAX;

	// Calculate the value
	const uint8_t value = ceil(percentage * MAX_CALC_VALUE);
				
	// Draw Image
	QImage image(1, MAX_CALC_VALUE, QImage::Format_RGB888);
	image.fill(BLACK_COLOR);
	
	for (int i = 0; i < MAX_CALC_VALUE; i++)
	{
		QColor color = BLACK_COLOR;
		uint8_t position = MAX_CALC_VALUE - i;

		if (position < safeValue)
			color = safeColor;
		else if (position < warnValue)
			color = warnColor;
		else
			color = hotColor;
		
		if (position < value)
			image.setPixelColor(0, i, color);
		else
			image.setPixelColor(0, i, BLACK_COLOR);
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
			// Making this write as BGR allows the rest of the values to come in as RGB
			imageData.append((char)qBlue(scanline[j]));
			imageData.append((char)qGreen(scanline[j]));
			imageData.append((char)qRed(scanline[j]));
		}
	}
		
	memcpy(finalImage.memptr(), imageData.data(), imageData.size());

	emit newFrame(finalImage);
}

Logger* AudioGrabber::getLog()
{
	return this->_log;
}

bool AudioGrabber::startAudio()
{
	this->started = true;
	return false;
}

void AudioGrabber::stopAudio()
{
	this->started = false;
}

