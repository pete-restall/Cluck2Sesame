#include <xc.h>
#include <stdint.h>

#include "../Platform/Event.h"
#include "../Platform/Buttons.h"

#include "Ui.h"

#define LEFT_BUTTON 0x02
#define RIGHT_BUTTON 0x01

static void uiInputIncrementScreenCharacter(void);
static void uiInputOnEnter(void);

const struct ButtonBehaviour uiInputIgnored = { };

const struct ButtonBehaviour uiInputIncrementRange =
{
	.onPressed = &uiInputIncrementScreenCharacter
};

const struct ButtonBehaviour uiInputEntered =
{
	.onReleased = &uiInputOnEnter
};

const struct ButtonsBehaviour uiInputUninitialised =
{
	.left = &uiInputIgnored,
	.right = &uiInputIgnored
};

const struct ButtonsBehaviour uiInputIsRange =
{
	.left = &uiInputIncrementRange,
	.right = &uiInputEntered
};

void uiInputOnButtonsPressed(const struct Event *event)
{
	const struct ButtonsPressed *pressed = (const struct ButtonsPressed *) event->args;

	if (pressed->mask & LEFT_BUTTON)
		uiState.flags.bits.isLeftButtonPressed = 1;

	if (pressed->mask & RIGHT_BUTTON)
		uiState.flags.bits.isRightButtonPressed = 1;

	if (!uiState.flags.bits.isScreenOn)
	{
		uiScreenOn();
		uiState.flags.bits.isButtonPressTurningOnScreen = 1;
		uiState.flags.bits.isScreenTimeoutDisabled = 1;
		return;
	}

	uiState.flags.bits.isButtonPressTurningOnScreen = 0;
	uiState.flags.bits.isScreenTimeoutDisabled = 1;

	if ((pressed->mask & LEFT_BUTTON) && uiState.input.buttons->left->onPressed)
		uiState.input.buttons->left->onPressed();

	if ((pressed->mask & RIGHT_BUTTON) && uiState.input.buttons->right->onPressed)
		uiState.input.buttons->right->onPressed();
}

void uiInputOnButtonsReleased(const struct Event *event)
{
	const struct ButtonsReleased *released = (const struct ButtonsReleased *) event->args;

	if (released->mask & LEFT_BUTTON)
	{
		uiState.flags.bits.isLeftButtonPressed = 0;
		if (!uiState.flags.bits.isButtonPressTurningOnScreen && uiState.input.buttons->left->onReleased)
			uiState.input.buttons->left->onReleased();
	}

	if (released->mask & RIGHT_BUTTON)
	{
		uiState.flags.bits.isRightButtonPressed = 0;
		if (!uiState.flags.bits.isButtonPressTurningOnScreen && uiState.input.buttons->right->onReleased)
			uiState.input.buttons->right->onReleased();
	}

	if (!uiState.flags.bits.isLeftButtonPressed && !uiState.flags.bits.isRightButtonPressed)
		uiScreenStartTimeout();
}

static void uiInputIncrementScreenCharacter(void)
{
	if (uiState.input.cursorPosition >= sizeof(uiState.screen))
		return;

	if (++uiState.screen[uiState.input.cursorPosition] > uiState.input.menu.range.max)
		uiState.screen[uiState.input.cursorPosition] = uiState.input.menu.range.min;

	uiScreenBlit();
}

static void uiInputOnEnter(void)
{
	if (uiState.input.onEnter)
		uiState.input.onEnter();
}
