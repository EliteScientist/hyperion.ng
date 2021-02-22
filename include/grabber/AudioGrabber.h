#pragma once

#include <QObject>

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

		AudioGrabber(const QString& device);

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
		void processAudioFrame(int16_t* buffer, int length);
};
