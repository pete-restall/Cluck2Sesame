#include <xc.h>
#include <stdint.h>

#include "NvmSettings.h"

#define PWM_VOLTS(x) ((uint8_t) ((256 * (x) / 3.3 + 0.5)))

#define DAC_VOLTS(x) ((uint8_t) ((32 * (x) / 3.3 + 0.5)))
#define DAC_VOLTS_PER_AMP (3 / 1.8)
#define DAC_AMPS(x) DAC_VOLTS((x) * DAC_VOLTS_PER_AMP)

__section("NvmSettings") const union NvmSettings nvmSettings =
{
	.platform =
	{
		.lcd =
		{
			.contrast = PWM_VOLTS(1.3),
			.backlightBrightness = PWM_VOLTS(1.65)
		},
		.motor =
		{
			.currentLimit = DAC_AMPS(1.2)
		}
	}
};
