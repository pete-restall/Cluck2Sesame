#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "Ui.h"

void uiEnterInitialDateAndTime(void)
{
	memcpy(
		uiState.screen,
		"Today is...     \0"
		"20YY-MM-DD hh:mm",
		sizeof(uiState.screen));

	uiState.cursorPositionY = 1;
	uiState.cursorPositionX = 2;
	uiScreenBlit();
}
