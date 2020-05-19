#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "../Platform/NvmSettings.h"

#include "../Door.h"

#include "Ui.h"

#define UP_POS UI_CURSOR_AT(0, 1)
#define DOWN_POS UI_CURSOR_AT(5, 1)
#define DONE_POS UI_CURSOR_AT(11, 1)

static void uiDoorControlScreenOptionSelected(void);
static void uiDoorControlScreenOptionAfterSelected(void);

void uiDoorCalibrationScreen(void)
{
	memcpy(
		uiState.screen,
		"Move door to top\0"
		">UP   DN    DONE",
		sizeof(uiState.screen));

	uiState.input.menu.options.cursorPositions[0] = UP_POS;
	uiState.input.menu.options.cursorPositions[1] = DOWN_POS;
	uiState.input.menu.options.cursorPositions[2] = DONE_POS;
	uiState.input.cursorPosition = UP_POS;
	uiState.input.selectedOptionIndex = 0;
	uiState.input.buttons = &uiInputIsOptions;
	uiState.input.onPreEnter = &uiDoorControlScreenOptionSelected;
	uiState.input.onEnter = &uiDoorControlScreenOptionAfterSelected;

	uiScreenBlit();
}

static void uiDoorControlScreenOptionSelected(void)
{
	// TODO: COULD DO WITH CAPTURING DOOR_ABORTED AND THEN SHOWING AN ALERT SCREEN WITH A MESSAGE FOR A FAULT ('JAMMED/TOO HEAVY', 'SPOOL REVERSED', etc.)  PROBABLY A BACKLIGHT FLASH AS WELL.  ALERT SCREEN WILL BE GENERALLY USEFUL, SO MAKE A 'uiAlertScreen.c' MODULE FOR IT.
	if (uiState.input.selectedOptionIndex == 0)
	{
		doorManualStartOpening();
		uiState.flags.bits.isDoorBeingManuallyControlled = 1;
	}

	if (uiState.input.selectedOptionIndex == 1)
	{
		doorManualStartClosing();
		uiState.flags.bits.isDoorBeingManuallyControlled = 1;
	}
}

static void uiDoorControlScreenOptionAfterSelected(void)
{
	if (uiState.flags.bits.isDoorBeingManuallyControlled)
	{
		doorManualStop();
		uiState.flags.bits.isDoorBeingManuallyControlled = 0;
	}

	if (uiState.input.selectedOptionIndex == 2)
	{
		// TODO: DO WE NEED TO CHECK FOR THE TRANSITION (IE. AN OPEN / CLOSE SCHEDULE WAS ACTIONED WHILST WE WERE DOING THE MANUAL CONTROL), THEN ACTION IT IF THE NVM'S MODE IS NOT 'MANUAL' ?
		static struct DoorStateWithContext x;
		doorGetState(&x);
		memcpy(
			uiState.screen,
			"ALL DONE...     \0"
			"Now do the rest.",
			sizeof(uiState.screen));

		uiState.screen[13] = '0' + x.current;
		uiState.screen[14] = '0' + x.transition;
		uiState.screen[15] = '0' + x.fault.all;
		uiState.input.buttons = &uiInputIsUninitialised;
		uiScreenBlit();
	}
}
