#ifndef __CLUCK3SESAME_SRC_PLATFORM_NVMSETTINGS_H
#define __CLUCK3SESAME_SRC_PLATFORM_NVMSETTINGS_H
#include <stdint.h>
#include "../ApplicationNvmSettings.h"

union NvmSettings
{
	uint8_t raw[32];
	struct
	{
		union
		{
			uint8_t raw[16];
			struct
			{
				struct
				{
					uint8_t contrast;
					uint8_t backlightBrightness;
				} lcd;

				struct
				{
					uint8_t currentLimit;
				} motor;
			};
		} platform;

		union ApplicationNvmSettings application;
	};
};

extern __section("NvmSettings") const union NvmSettings nvmSettings;

#endif
