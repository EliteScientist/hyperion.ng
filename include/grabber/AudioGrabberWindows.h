#pragma once

#include <QObject>

// Hyperion-utils includes
#include <grabber/AudioGrabber.h>

///
/// @brief The Windows Audio capture implementation
///
class AudioGrabberWindows : public AudioGrabber
{
	public:

		AudioGrabberWindows(const QString& device);

		bool startAudio();
		void stopAudio();
		QObjectList listAudioDevices();
		

	private:
		///
		/// @brief free the _screen pointer
		///
		void freeResources();
					
		int16_t grabAudioFrame(int16_t* buffer);

	private:
		QObjectList _audioDevices;

};
