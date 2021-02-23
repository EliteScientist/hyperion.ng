#pragma once

#include <QObject>

// Hyperion-utils includes
#include <grabber/AudioGrabber.h>

class AudioGrabberLinux : public AudioGrabber
{
	public:

		AudioGrabberLinux(const QString& device);


	public slots:
		bool startAudio() override;
		void stopAudio() override;

		///
		/// @brief overwrite Grabber.h implementation
		///
		QStringList getDevices() const override;

		///
		/// @brief overwrite Grabber.h implementation
		///
		QString getDeviceName(const QString& devicePath) const override;

		///
		/// @brief overwrite Grabber.h implementation
		///
		QMultiMap<QString, int> getDeviceInputs(const QString& devicePath) const override;


	private:
		///
		/// @brief free the _screen pointer
		///
		void freeResources();
		void refreshDevices();
		bool configureCaptureInterface();

	public:
		std::atomic<bool> isRunning{ false };
};
