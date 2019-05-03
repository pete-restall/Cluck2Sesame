#include <xc.h>

#include "NvmSettings.h"

__section("NvmSettings") const union NvmSettings nvmSettings =
{
	.lcd =
	{
		.backlightBrightness = 0x80
	}
};
