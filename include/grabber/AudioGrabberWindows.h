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

		AudioGrabberWindows(const QString& device);


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
		void processAudioBuffer();

		LPDIRECTSOUNDCAPTURE8 recordingDevice;
		LPDIRECTSOUNDCAPTUREBUFFER8 recordingBuffer;

		HANDLE notificationEvent;
		DWORD bufferCapturePosition;
		DWORD bufferCaptureSize;
		DWORD notificationSize;
		

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

			// Process Device Information
			QString deviceName = QString::fromUtf8(deviceDescStr);

			Debug(Logger::getInstance("AudioGrabber"), "Found Audio Device: %s = %s", deviceIdStr, deviceDescStr);

			
			CoTaskMemFree(deviceIdStr);

			device.name = deviceName;

			devices->insert(deviceId, device);

			return TRUE;
		}

};
