#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "NvmSettingsFixture.h"

#include "NvmSettings.h"
#include "ApplicationNvmSettings.h"

#define RETLW_HIGH 0b110100

static void nvmUnlockSequence(void);

void stubNvmApplicationSettings(const union ApplicationNvmSettings *const settings)
{
	static union NvmSettings replacementSettings;
	memcpy(
		&replacementSettings.platform,
		&nvmSettings.platform,
		sizeof(nvmSettings.platform));

	memcpy(
		&replacementSettings.application,
		settings,
		sizeof(nvmSettings.application));

	static const uint8_t *dest = (const uint8_t *) &nvmSettings;
	NVMADR = ((uint16_t) dest) & 0x7fff;
	NVMCON1bits.NVMREGS = 0;
	NVMCON1bits.FREE = 1;
	NVMCON1bits.WREN = 1;
	nvmUnlockSequence();

	NVMCON1bits.LWLO = 1;
	NVMDATH = RETLW_HIGH;
	const uint8_t *src = (const uint8_t *) &replacementSettings;
	for (uint8_t i = 0; i < sizeof(nvmSettings) - 1; i++)
	{
		NVMDATL = *(src++);
		nvmUnlockSequence();
		NVMADR++;
	}

	NVMCON1bits.LWLO = 0;
	NVMDATL = *src;
	nvmUnlockSequence();
	NVMCON1bits.WREN = 0;
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
