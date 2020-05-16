#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "Ui.h"

void uiEnterInitialDateAndTime(void)
{
	memcpy(
		uiState.screen,
		"Today is...     \0"
		"2020-01-01 00:00",
		sizeof(uiState.screen));

	uiState.input.cursorPosition = UI_CURSOR_AT(2, 1);
	uiState.input.menu.range.min = '2';
	uiState.input.menu.range.max = '9';
	uiState.input.buttons = &uiInputIsRange;
	uiState.input.onEnter = 0; // TODO...
	uiScreenBlit();
}
