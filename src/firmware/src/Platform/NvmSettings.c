#include <xc.h>
#include <stdint.h>

#include "NvmSettings.h"
#include "../ApplicationNvmSettings.h"

#define PWM_VOLTS(x) ((uint8_t) ((256 * (x) / 3.3 + 0.5)))

#define DAC_VOLTS(x) ((uint8_t) ((32 * (x) / 3.3 + 0.5)))
#define DAC_VOLTS_PER_AMP (3 / 1.8)
#define DAC_AMPS(x) DAC_VOLTS((x) * DAC_VOLTS_PER_AMP)

__section("NvmSettings") const volatile union NvmSettings nvmSettings =
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
			.currentLimitNoLoad = DAC_AMPS(0.24),
			.currentLimitMaximumLoad = DAC_AMPS(1.2)
		}
	},// TODO: ALL OF THE APPLICATION SETTINGS ARE TEMPORARY, FOR BOARD-LEVEL DEBUGGING...
	.application =
	{
		.door =
		{
			.mode =
			{
				.isManuallyOverridden = 1
			},
			.height = 150
		}
	}
};
