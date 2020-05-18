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
	if (uiState.input.selectedOptionIndex == 0)
		doorManualStartOpening();

	if (uiState.input.selectedOptionIndex == 1)
		doorManualStartClosing();
}

static void uiDoorControlScreenOptionAfterSelected(void)
{
	if (uiState.input.selectedOptionIndex != 2)
	{
		// TODO: CAN GET WEIRDNESS HERE - IF HELD DOWN RIGHT BUTTON THEN PRESSED LEFT BUTTON BEFORE RELEASING RIGHT BUTTON THEN THE OPTION WILL MOVE ALONG...THINK ABOUT THIS
		doorManualStop();
	}
	else
	{
		// TODO: DO WE NEED TO CHECK FOR THE TRANSITION (IE. AN OPEN / CLOSE SCHEDULE WAS ACTIONED WHILST WE WERE DOING THE MANUAL CONTROL), THEN ACTION IT IF THE NVM'S MODE IS NOT 'MANUAL' ?
		memcpy(
			uiState.screen,
			"ALL DONE...     \0"
			"Now do the rest.",
			sizeof(uiState.screen));

		uiState.input.buttons = &uiInputIsUninitialised;
		uiScreenBlit();
	}
}
