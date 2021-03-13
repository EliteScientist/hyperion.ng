#pragma once
#include <pthread.h>
#include <alsa/asoundlib.h>
#include <QObject>

// Hyperion-utils includes
#include <grabber/AudioGrabber.h>

class AudioGrabberLinux : public AudioGrabber
{
	public:

		AudioGrabberLinux(const QString& device, const QJsonObject& config);
		virtual ~AudioGrabberLinux();
		std::atomic<bool> isRunning{ false };
		void processAudioBuffer(int frames);

		snd_pcm_t * captureDevice;
		snd_pcm_hw_params_t * captureDeviceConfig;

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
		unsigned int sampleRate = 44100;

		pthread_t audioThread;
};

static void* AudioThreadRunner(void* params);
