#include <xc.h>
#include <stdint.h>

#include "../Platform/NvmSettings.h"
#include "../Platform/NearScheduler.h"
#include "../Platform/Lcd.h"

#include "Ui.h"

static void uiOnScreenTimeout(void *state);

static const struct NearSchedule uiScreenTimeoutSchedule =
{
	.ticks = MS_TO_TICKS(1000),
	.handler = &uiOnScreenTimeout
};

void uiScreenOn(void)
{
	if (!uiState.flags.bits.isScreenOn)
	{
		lcdEnable();
		uiState.flags.bits.isScreenOn = 1;
		uiState.flags.bits.isScreenTimeoutDisabled = 0;
		nearSchedulerAddOrUpdate(&uiScreenTimeoutSchedule);
	}
}

void uiScreenOff(void)
{
	if (uiState.flags.bits.isScreenOn)
	{
		uiState.flags.bits.isScreenOn = 0;
		lcdDisable();
	}
}

void uiScreenStartTimeout(void)
{
	uiState.flags.bits.isScreenTimeoutDisabled = 0;
	nearSchedulerAddOrUpdate(&uiScreenTimeoutSchedule);
}

static void uiOnScreenTimeout(void *state)
{
	static uint8_t timeoutCount = 0;
	if (uiState.flags.bits.isScreenTimeoutDisabled)
		timeoutCount = 0;

	if (++timeoutCount == nvmSettings.application.ui.screenTimeoutSeconds)
	{
		uiScreenOff();
		timeoutCount = 0;
	}
	else
		nearSchedulerAddOrUpdate(&uiScreenTimeoutSchedule);
}
