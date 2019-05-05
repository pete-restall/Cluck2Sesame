#include <xc.h>

#include "NvmSettings.h"

__section("NvmSettings") const union NvmSettings nvmSettings =
{
	.lcd =
	{
		.contrast = 0x26,
		.backlightBrightness = 0x80
	}
};
