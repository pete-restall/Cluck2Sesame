#ifndef __CLUCK2SESAME_SRC_PLATFORM_NVMSETTINGS_H
#define __CLUCK2SESAME_SRC_PLATFORM_NVMSETTINGS_H
#include <stdint.h>
#include "Event.h"
#include "../ApplicationNvmSettings.h"

#define NVM_SETTINGS_CHANGED ((EventType) 0x30)
struct NvmSettingsChanged { EMPTY_EVENT_ARGS };

union NvmSettings
{
	uint8_t raw[32];
	struct
	{
		union
		{
			uint8_t raw[15];
			struct
			{
				struct
				{
					uint8_t contrast;
					uint8_t backlightBrightness;
				} lcd;

				struct
				{
					uint8_t currentLimitNoLoad;
					uint8_t currentLimitMaximumLoad;
				} motor;

				struct
				{
					uint16_t temperatureHighAdc;
					uint8_t temperatureHighCelsius;
					uint16_t temperatureCoefficient;
				} temperature;

				struct
				{
					uint16_t placeholder[2]; // TODO: FOR CALIBRATION PURPOSES
				} crystal;

				union
				{
					uint8_t all;
					struct
					{
						unsigned int isCalibrationRequired : 1;
					} bits;
				} flags;
			};
		} platform;

		union ApplicationNvmSettings application;

		uint8_t crc8;
	};
};

extern uint8_t nvmSettingsStore(const union NvmSettings *newSettings);

extern __section("NvmSettings") const volatile union NvmSettings nvmSettings;

#endif
