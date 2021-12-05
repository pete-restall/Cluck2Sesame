#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "Ui.h"

void uiCalibrationModeScreen(void)
{
	memcpy(
		uiState.screen,
		"CALIBRATION MODE\0"
		"   (HEADLESS)   ",
		sizeof(uiState.screen));

	uiState.flags.bits.isButtonPressPreventedFromTurningOnScreen = 1;
	uiState.input.buttons = &uiInputIsUninitialised;
	uiScreenBlit();
	uiScreenStartTimeout();
}
