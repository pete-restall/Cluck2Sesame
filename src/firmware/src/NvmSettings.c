#include <xc.h>

#include "NvmSettings.h"

__section("NvmSettings") const struct NvmSettings nvmSettings =
{
	.lcd =
	{
		.backlightBrightness = 0x80
	}
};
