#include <xc.h>
#include <stdint.h>

#include "../Platform/Event.h"
#include "../Platform/Buttons.h"

#include "Ui.h"

#define LEFT_BUTTON 0x02
#define RIGHT_BUTTON 0x01

void uiOnButtonsPressed(const struct Event *event)
{
	const struct ButtonsPressed *pressed = (const struct ButtonsPressed *) event->args;

	if (pressed->mask & LEFT_BUTTON)
		uiState.flags.bits.isLeftButtonPressed = 1;

	if (pressed->mask & RIGHT_BUTTON)
		uiState.flags.bits.isRightButtonPressed = 1;

	if (!uiState.flags.bits.isScreenOn)
	{
		uiScreenOn();
		uiState.flags.bits.isScreenTimeoutDisabled = 1;
		return;
	}

	uiState.flags.bits.isScreenTimeoutDisabled = 1;
//	switch (uiState.menu.inputStyle)
//	{
//		case UiInputStyle.
//	}
	// TODO: menu stuff
}

void uiOnButtonsReleased(const struct Event *event)
{
	const struct ButtonsReleased *released = (const struct ButtonsReleased *) event->args;

	if (released->mask & LEFT_BUTTON)
		uiState.flags.bits.isLeftButtonPressed = 0;

	if (released->mask & RIGHT_BUTTON)
		uiState.flags.bits.isRightButtonPressed = 0;

	if (!uiState.flags.bits.isLeftButtonPressed && !uiState.flags.bits.isRightButtonPressed)
		uiScreenStartTimeout();
}
