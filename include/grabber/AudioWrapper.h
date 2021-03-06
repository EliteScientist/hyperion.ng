#pragma once

#include <hyperion/GrabberWrapper.h>

#ifdef WIN32
	#include <grabber/AudioGrabberWindows.h>
#endif

#ifdef __linux__
	#include <grabber/AudioGrabberLinux.h>
#endif



class AudioWrapper : public GrabberWrapper
{
	public:
		///
		/// Constructs the Audio grabber with a specified grab size and update rate.
		///
		/// @param[in] device			Audio Device Identifier
		/// @param[in] updateRate_Hz	The audio grab rate [Hz]
		///
		AudioWrapper(const QString& device, const QJsonObject& config);

		///
		/// Destructor of this Audio grabber. Releases any claimed resources.
		///
		virtual ~AudioWrapper();

		void handleSettingsUpdate(settings::type type, const QJsonDocument& config);

	public slots:
		///
		/// Performs a single frame grab and computes the led-colors
		///
		void action() override;
		bool start() override;
		void stop() override;

	private:
		void newFrame(const Image<ColorRgb>& image);

		/// The actual grabber
#ifdef WIN32
		AudioGrabberWindows _grabber;
#endif

#ifdef __linux__
		AudioGrabberLinux _grabber;
#endif

};
