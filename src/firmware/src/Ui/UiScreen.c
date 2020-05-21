#include <xc.h>
#include <stdint.h>
#include <stdlib.h>

#include "../Platform/NvmSettings.h"
#include "../Platform/NearScheduler.h"
#include "../Platform/Lcd.h"

#include "Ui.h"

static void uiOnScreenTimeout(void *state);
static void uiScreenSetAddressToHome(void *state);
static void uiScreenBlitFirstLine(void *state);
static void uiScreenGotoSecondLine(void *state);
static void uiScreenBlitSecondLine(void *state);
static void uiScreenSetCursorPosition(void *state);
static void uiScreenTurnCursorOn(void *state);
static void uiScreenBlitDone(void *state);

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
	uiState.flags.bits.isScreenBlitDirty = 0;
	uiState.flags.bits.isScreenBeingBlitted = 0;

	if (uiState.flags.bits.isScreenOn)
	{
		uiState.flags.bits.isScreenOn = 0;
		lcdDisable();
	}
}

void uiScreenStartTimeout(void)
{
	uiState.flags.bits.isScreenTimeoutDisabled = 0;
	uiState.screenTimeoutCount = 0;
	nearSchedulerAddOrUpdate(&uiScreenTimeoutSchedule);
}

static void uiOnScreenTimeout(void *state)
{
	if (uiState.flags.bits.isScreenTimeoutDisabled)
		uiState.screenTimeoutCount = 0;

	if (++uiState.screenTimeoutCount == nvmSettings.application.ui.screenTimeoutSeconds)
	{
		uiScreenOff();
		uiState.screenTimeoutCount = 0;
	}
	else
		nearSchedulerAddOrUpdate(&uiScreenTimeoutSchedule);
}

void uiScreenBlit(void)
{
	if (!uiState.flags.bits.isLcdEnabled || uiState.flags.bits.isScreenBeingBlitted)
	{
		uiState.flags.bits.isScreenBlitDirty = uiState.flags.bits.isScreenBeingBlitted ? 1 : 0;
		return;
	}

	uiState.flags.bits.isScreenBlitDirty = 0;
	uiState.flags.bits.isScreenBeingBlitted = 1;

	static const struct LcdSetCursorTransaction transaction =
	{
		.on = 0,
		.callback = &uiScreenSetAddressToHome
	};

	lcdSetCursor(&transaction);
}

static void uiScreenSetAddressToHome(void *state)
{
	if (!uiState.flags.bits.isLcdEnabled)
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
	if (!uiState.flags.bits.isLcdEnabled)
		return;

	uiState.flags.bits.isScreenBlitDirty = 0;

	static const struct LcdPutsTransaction transaction =
	{
		.buffer = &uiState.screen[0],
		.callback = &uiScreenGotoSecondLine
	};

	lcdPuts(&transaction);
}

static void uiScreenGotoSecondLine(void *state)
{
	if (!uiState.flags.bits.isLcdEnabled)
		return;

	static const struct LcdSetAddressTransaction transaction =
	{
		.address = LCD_ADDRESS_LINE2,
		.callback = &uiScreenBlitSecondLine
	};

	lcdSetDdramAddress(&transaction);
}

static void uiScreenBlitSecondLine(void *state)
{
	if (!uiState.flags.bits.isLcdEnabled)
		return;

	static const struct LcdPutsTransaction transaction =
	{
		.buffer = &uiState.screen[UI_SCREEN_WIDTH + 1],
		.callback = &uiScreenSetCursorPosition
	};

	lcdPuts(&transaction);
}

static void uiScreenSetCursorPosition(void *state)
{
	if (!uiState.flags.bits.isLcdEnabled)
		return;

	struct LcdSetAddressTransaction transaction =
	{
		.address = uiState.input.cursorPosition,
		.callback = &uiScreenTurnCursorOn
	};

	if (transaction.address > UI_SCREEN_WIDTH)
	{
		transaction.address -= (UI_SCREEN_WIDTH + 1);
		transaction.address += LCD_ADDRESS_LINE2;
	}

	if (transaction.address < LCD_ADDRESS_LINE2 + UI_SCREEN_WIDTH)
		lcdSetDdramAddress(&transaction);
	else
		uiScreenBlitDone((void *) 0);
}

static void uiScreenTurnCursorOn(void *state)
{
	if (!uiState.flags.bits.isLcdEnabled)
		return;

	static const struct LcdSetCursorTransaction transaction =
	{
		.on = 1,
		.callback = &uiScreenBlitDone
	};

	lcdSetCursor(&transaction);
}

static void uiScreenBlitDone(void *state)
{
	uiState.flags.bits.isScreenBeingBlitted = 0;
	if (uiState.flags.bits.isScreenBlitDirty)
		uiScreenBlit();
}

int8_t uiScreenSignAndTwoDigitsFromPosition(uint8_t cursorPosition)
{
	int8_t value = (int8_t) uiScreenTwoDigitsFromPosition(cursorPosition + 1);
	if (uiState.screen[cursorPosition] == '-')
		return -value;

	return value;
}

uint8_t uiScreenTwoDigitsFromPosition(uint8_t cursorPosition)
{
	return
		(uiState.screen[cursorPosition] - '0') * 10 +
		(uiState.screen[cursorPosition + 1] - '0');
}

void uiScreenTwoDigitsToPosition(uint8_t cursorPosition, uint8_t value)
{
	div_t digits;
	digits = div(value, 10);
	uiState.screen[cursorPosition] = (char) ('0' + digits.quot);
	uiState.screen[cursorPosition + 1] = (char) ('0' + digits.rem);
}
