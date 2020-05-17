#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "Ui.h"

void uiLatitudeAndLongitudeEntryScreen(void)
{
	memcpy(
		uiState.screen,
		"Lat-Long is...  \0"
		"+5y.yy +xx.xx   ",
		sizeof(uiState.screen));

	// TODO...
	uiState.input.buttons = &uiInputIsUninitialised;
	uiScreenBlit();
}
