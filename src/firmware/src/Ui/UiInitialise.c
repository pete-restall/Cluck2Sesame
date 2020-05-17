#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "../Platform/Event.h"
#include "../Platform/Main.h"
#include "../Platform/Buttons.h"
#include "../Platform/Lcd.h"
#include "../Platform/NvmSettings.h"

#include "Ui.h"

static void uiOnLcdEnabled(const struct Event *event);
static void uiOnLcdDisabled(const struct Event *event);

struct UiState uiState;
union NvmSettings uiNvmSettings;

void uiInitialise(void)
{
	static const struct EventSubscription onButtonsPressedSubscription =
	{
		.type = BUTTONS_PRESSED,
		.handler = &uiInputOnButtonsPressed,
		.state = (void *) 0
	};

	eventSubscribe(&onButtonsPressedSubscription);

	static const struct EventSubscription onButtonsReleasedSubscription =
	{
		.type = BUTTONS_RELEASED,
		.handler = &uiInputOnButtonsReleased,
		.state = (void *) 0
	};

	eventSubscribe(&onButtonsReleasedSubscription);

	static const struct EventSubscription onSystemInitialisedSubscription =
	{
		.type = SYSTEM_INITIALISED,
		.handler = &uiOnSystemInitialised,
		.state = (void *) 0
	};

	eventSubscribe(&onSystemInitialisedSubscription);

	static const struct EventSubscription onLcdEnabledSubscription =
	{
		.type = LCD_ENABLED,
		.handler = &uiOnLcdEnabled
	};

	eventSubscribe(&onLcdEnabledSubscription);

	static const struct EventSubscription onLcdDisabledSubscription =
	{
		.type = LCD_DISABLED,
		.handler = &uiOnLcdDisabled
	};

	eventSubscribe(&onLcdDisabledSubscription);

	uiState.input.buttons = &uiInputIsUninitialised;
	uiState.input.cursorPosition = UI_NO_CURSOR;

	memcpy(&uiNvmSettings, (const void *) &nvmSettings, sizeof(union NvmSettings));
}

static void uiOnLcdEnabled(const struct Event *event)
{
	uiState.flags.bits.isLcdEnabled = 1;
	uiScreenBlit();
}

static void uiOnLcdDisabled(const struct Event *event)
{
	uiState.flags.bits.isLcdEnabled = 0;
}
