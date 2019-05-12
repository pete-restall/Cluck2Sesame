#include <xc.h>

#include "NvmSettings.h"

#define VOLTS(x) ((uint8_t) ((256 * (x) / 3.3 + 0.5)))

__section("NvmSettings") const union NvmSettings nvmSettings =
{
	.lcd =
	{
		.contrast = VOLTS(1.3),
		.backlightBrightness = VOLTS(1.65)
	}
};
