#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "Ui.h"

#define OK_NEXT_LINE " OK        >NEXT"
#define OK_POS UI_CURSOR_AT(0, 1)
#define NEXT_POS UI_CURSOR_AT(11, 1)

static void uiMenuScreenOptionSelected(void);

void uiMenuSettingsScreenmenu(void)
{
	uiState.menu.itemText = "Change Settings ";
	uiState.menu.onNext = &uiDateAndTimeStatusScreen;
	uiState.menu.onOk = &UI_FIRST_SETTINGS_SCREEN;
	uiMenuScreen();
}

void uiMenuScreen(void)
{
	if (uiState.menu.itemText)
	{
		memcpy(
			&uiState.screen[0],
			uiState.menu.itemText,
			UI_SCREEN_WIDTH + 1);
	}

	memcpy(
		&uiState.screen[UI_SCREEN_WIDTH + 1],
		OK_NEXT_LINE,
		UI_SCREEN_WIDTH + 1);

	uiState.input.menu.options.cursorPositions[0] = OK_POS;
	uiState.input.menu.options.cursorPositions[1] = NEXT_POS;
	uiState.input.menu.options.cursorPositions[2] = UI_NO_CURSOR;
	uiState.input.cursorPosition = NEXT_POS;
	uiState.input.selectedOptionIndex = 1;
	uiState.input.buttons = &uiInputIsOptions;
	uiState.input.onPreEnter = 0;
	uiState.input.onEnter = &uiMenuScreenOptionSelected;

	uiScreenBlit();
}

static void uiMenuScreenOptionSelected(void)
{
	if (uiState.input.selectedOptionIndex == 0)
	{
		if (uiState.menu.onOk)
			uiState.menu.onOk();
	}
	else
	{
		if (uiState.menu.onNext)
			uiState.menu.onNext();
	}
}

void uiMenuSettingsBackMenuScreen(void)
{
	uiState.menu.itemText = "Back            ";
	uiState.menu.onNext = &UI_FIRST_SETTINGS_SCREEN;
	uiState.menu.onOk = &UI_DEFAULT_SCREEN;
	uiMenuScreen();
}
