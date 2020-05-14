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
			.currentLimitNoLoad = DAC_AMPS(0.415), // TODO: PEAK 415mA, AVERAGE 140mA WHEN UNLOADED @ 4.2V...*HOWEVER*, THIS CHANGES FOR LOWER VOLTAGES...NEED TO ADAPT THIS PARAMETER ACCORDING TO MOTOR VOLTAGE !
			.currentLimitMaximumLoad = DAC_AMPS(1.0) // TODO: THIS MAY ALSO HAVE A VOLTAGE DEPENDENCY AS WELL, ALTHOUGH THE WINDING RESISTANCE MAY BE THE LIMITING FACTOR...TEST IT...
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
			.height = 4500
		},
		.ui =
		{
			.screenTimeoutSeconds = 20
		}
	}
};
