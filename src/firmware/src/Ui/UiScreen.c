#include <xc.h>
#include <stdint.h>

#include "../Platform/NvmSettings.h"
#include "../Platform/NearScheduler.h"
#include "../Platform/Lcd.h"

#include "Ui.h"

static void uiOnScreenTimeout(void *state);
static void uiScreenBlitFirstLine(void *state);
static void uiScreenGotoSecondLine(void *state);
static void uiScreenBlitSecondLine(void *state);

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

void uiScreenBlit(void)
{
	if (!uiState.flags.bits.isScreenOn)
		return;

	static const struct LcdSetAddressTransaction transaction =
	{
		.address = 0x00,
		.callback = &uiScreenBlitFirstLine
	};

	lcdSetDdramAddress(&transaction);
}

static void uiScreenBlitFirstLine(void *state)
{
	if (!uiState.flags.bits.isScreenOn)
		return;

	static const struct LcdPutsTransaction transaction =
	{
		.buffer = uiState.screen[0],
		.callback = &uiScreenGotoSecondLine
	};

	lcdPuts(&transaction);
}

static void uiScreenGotoSecondLine(void *state)
{
	if (!uiState.flags.bits.isScreenOn)
		return;

	static const struct LcdSetAddressTransaction transaction =
	{
		.address = 0x40,
		.callback = &uiScreenBlitSecondLine
	};

	lcdSetDdramAddress(&transaction);
}

static void uiScreenBlitSecondLine(void *state)
{
	if (!uiState.flags.bits.isScreenOn)
		return;

	// TODO........
	static const struct LcdPutsTransaction transaction =
	{
		.buffer = uiState.screen[1],
		.callback = 0 //uiState.onScreenBlitted
	};

	lcdPuts(&transaction);
	// uiState.onScreenBlitted = (LcdCallback) 0;
}
