#pragma once

#include <QObject>

// Hyperion-utils includes
#include <grabber/AudioGrabber.h>
#include <DSound.h>

#define AUDIO_NOTIFICATION_COUNT	4

///
/// @brief The Windows Audio capture implementation
///
class AudioGrabberWindows : public AudioGrabber
{
	public:

		AudioGrabberWindows(const QString& device, const QJsonObject& config);


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

		LPDIRECTSOUNDCAPTURE8 recordingDevice;
		LPDIRECTSOUNDCAPTUREBUFFER8 recordingBuffer;

		HANDLE audioThread;
		DWORD bufferCapturePosition;
		DWORD bufferCaptureSize;
		DWORD notificationSize;
		static DWORD WINAPI AudioThreadRunner(LPVOID param);

	public:
		HANDLE notificationEvent;
		std::atomic<bool> isRunning{ false };
		void processAudioBuffer();
		

	private:

		static BOOL CALLBACK DirectSoundEnumProcessor(LPGUID deviceIdGuid, LPCTSTR deviceDescStr,
			LPCTSTR deviceModelStr, LPVOID context)
		{
			QMap<QString, AudioGrabber::DeviceProperties>* devices = (QMap<QString, AudioGrabber::DeviceProperties>*)context;


			AudioGrabber::DeviceProperties device;

			// Process Device ID
			LPOLESTR deviceIdStr;
			StringFromCLSID(*deviceIdGuid, &deviceIdStr);

			QString deviceId = QString::fromWCharArray(deviceIdStr);
			CoTaskMemFree(deviceIdStr);

			// Process Device Information
			QString deviceName = QString::fromUtf8(deviceDescStr);

			Debug(Logger::getInstance("AudioGrabber"), "Found Audio Device: %s", deviceDescStr);
					
			
			device.name = deviceName;

			devices->insert(deviceId, device);

			return TRUE;
		}

};
