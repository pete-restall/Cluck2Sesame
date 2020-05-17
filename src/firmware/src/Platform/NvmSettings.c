#include <xc.h>
#include <stdint.h>

#include "Event.h"
#include "NvmSettings.h"
#include "../ApplicationNvmSettings.h"

#define RETLW_HIGH 0b110100

#define PWM_VOLTS(x) ((uint8_t) ((256 * (x) / 3.3 + 0.5)))

#define DAC_VOLTS(x) ((uint8_t) ((32 * (x) / 3.3 + 0.5)))
#define DAC_VOLTS_PER_AMP (3 / 1.8)
#define DAC_AMPS(x) DAC_VOLTS((x) * DAC_VOLTS_PER_AMP)

static void nvmUnlockSequence(void);
static uint8_t nvmCrc8Next(uint8_t crc8, uint8_t input);

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
	},
	.application =
	{
		.door =
		{
			.mode =
			{
				.isManuallyOverridden = 1
			},
			.height = 0
		},
		.ui =
		{
			.screenTimeoutSeconds = 20
		}
	},
	.crc8 = 0x00 // TODO: THIS NEEDS CALCULATING, AND IT NEEDS VERIFYING ON BOOT
};

void nvmSettingsStore(const union NvmSettings *newSettings)
{
	static const uint8_t *const dest = (const uint8_t *) &nvmSettings;
	NVMADR = ((uint16_t) dest) & 0x7fff;
	NVMCON1bits.NVMREGS = 0;
	NVMCON1bits.FREE = 1;
	NVMCON1bits.WREN = 1;
	nvmUnlockSequence();

	NVMCON1bits.LWLO = 1;
	NVMDATH = RETLW_HIGH;
	const uint8_t *src = (const uint8_t *) newSettings;
	uint8_t crc8 = 0;
	for (uint8_t i = 0; i < sizeof(nvmSettings) - 1; i++)
	{
		NVMDATL = *(src++);
		crc8 = nvmCrc8Next(crc8, NVMDATH);
		crc8 = nvmCrc8Next(crc8, NVMDATL);
		nvmUnlockSequence();
		NVMADR++;
	}

	NVMCON1bits.LWLO = 0;
	NVMDATL = crc8;
	nvmUnlockSequence();
	NVMCON1bits.WREN = 0;

	// if (NVMCON1bits.WRERR)
		// TODO: WHAT TO DO HERE ?  IT'S A FAULT BUT UNRECOVERABLE.  A REBOOT WILL BRING UP THE DEVICE IN AN INVALID STATE.

	eventPublish(NVM_SETTINGS_CHANGED, &eventEmptyArgs);
}

static void nvmUnlockSequence(void)
{
	uint8_t interruptsWereEnabled = INTCONbits.GIE;
	INTCONbits.GIE = 0;
	NVMCON2 = 0x55;
	NVMCON2 = 0xaa;
	NVMCON1bits.WR = 1;
	if (interruptsWereEnabled)
		INTCONbits.GIE = 1;
}

static uint8_t nvmCrc8Next(uint8_t crc8, uint8_t input)
{
	uint8_t i;
	input ^= crc8;
	for (i = 0; i < 8; i++)
	{
		uint8_t msb = input & 0x80;
		input <<= 1;
		if (msb != 0)
			input ^= 0x07;
	}

	return input;
}
