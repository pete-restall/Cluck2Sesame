#ifndef __CLUCK3SESAME_SRC_NVMSETTINGS_H
#define __CLUCK3SESAME_SRC_NVMSETTINGS_H
#include <stdint.h>

struct NvmSettings
{
	struct
	{
		uint8_t backlightBrightness;
	} lcd;
};

extern __section("NvmSettings") const struct NvmSettings nvmSettings;

#endif
