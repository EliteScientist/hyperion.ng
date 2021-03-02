#pragma once

#include <QObject>
#include <QColor>

// Hyperion-utils includes
#include <utils/ColorRgb.h>
#include <hyperion/Grabber.h>
#include <utils/Logger.h>

///
/// @brief The DirectX9 capture implementation
///
class AudioGrabber : public Grabber
{
	Q_OBJECT
	public:

		AudioGrabber(const QString& device, const QJsonObject& config);

		virtual ~AudioGrabber();

		///
		/// Captures a single snapshot of the display and writes the data to the given image. The
		/// provided image should have the same dimensions as the configured values (_width and
		/// _height)
		///
		/// @param[out] image  The snapped screenshot
		///
		virtual int grabFrame(Image<ColorRgb>& image);

		virtual bool startAudio();
		virtual void stopAudio();

		struct DeviceProperties
		{
			QString					name = QString();
			QMultiMap<QString, int>	inputs = QMultiMap<QString, int>();
		};

		Logger* getLog();

		void setDevicePath(const QString& device);
		void setConfiguration(const QJsonObject& config);

	private:
		///
		/// @brief free the _screen pointer
		///
		void freeResources();
		
	signals:
		void newFrame(const Image<ColorRgb>& image);

	protected:
		QMap<QString, AudioGrabber::DeviceProperties> _deviceProperties;
		Logger* _log;
		QString _device;

		const uint8_t MAX_CALC_VALUE = 100;

		// Default Empty Color
		const QColor BLACK_COLOR = QColor("#000000");

		QColor hotColor;
		uint8_t warnValue;
		QColor warnColor;
		uint8_t safeValue;
		QColor safeColor;
		uint16_t multiplier;

		bool started	= false;

		void processAudioFrame(int16_t* buffer, int length);
};
