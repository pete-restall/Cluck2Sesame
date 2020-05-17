#ifndef __CLUCK3SESAME_SRC_PLATFORM_NVMSETTINGS_H
#define __CLUCK3SESAME_SRC_PLATFORM_NVMSETTINGS_H
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
			};
		} platform;

		union ApplicationNvmSettings application;

		uint8_t crc8;
	};
};

extern void nvmSettingsStore(const union NvmSettings *newSettings);

extern __section("NvmSettings") const volatile union NvmSettings nvmSettings;

#endif
