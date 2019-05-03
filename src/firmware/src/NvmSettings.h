#ifndef __CLUCK3SESAME_SRC_NVMSETTINGS_H
#define __CLUCK3SESAME_SRC_NVMSETTINGS_H
#include <stdint.h>

union NvmSettings
{
	uint8_t raw[32];
	struct
	{
		struct
		{
			uint8_t backlightBrightness;
		} lcd;
	};
};

extern __section("NvmSettings") const union NvmSettings nvmSettings;

#endif
