#pragma once

#include <hyperion/GrabberWrapper.h>

#ifdef WIN32
	#include <grabber/AudioGrabberWindows.h>
#endif

#ifdef LINUX
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
		AudioWrapper(const QString& device, const unsigned updateRate_Hz);

		///
		/// Destructor of this Audio grabber. Releases any claimed resources.
		///
		virtual ~AudioWrapper() {};

	public slots:
		///
		/// Performs a single frame grab and computes the led-colors
		///
		virtual void action();

	private:
		/// The actual grabber
#ifdef WIN32
		AudioGrabberWindows _grabber;
#endif

#ifdef LINUX
		AudioGrabberLinux _grabber;
#endif

};
