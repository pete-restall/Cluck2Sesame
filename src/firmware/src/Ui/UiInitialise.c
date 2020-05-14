#include <xc.h>
#include <stdint.h>

#include "../Platform/Event.h"
#include "../Platform/Main.h"
#include "../Platform/Buttons.h"
#include "../Platform/Lcd.h"

#include "Ui.h"

struct UiState uiState;

void uiInitialise(void)
{
	static const struct EventSubscription onButtonsPressedSubscription =
	{
		.type = BUTTONS_PRESSED,
		.handler = &uiOnButtonsPressed,
		.state = (void *) 0
	};

	eventSubscribe(&onButtonsPressedSubscription);

	static const struct EventSubscription onButtonsReleasedSubscription =
	{
		.type = BUTTONS_RELEASED,
		.handler = &uiOnButtonsReleased,
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
		.handler = (EventHandler) &uiScreenBlit
	};

	eventSubscribe(&onLcdEnabledSubscription);
}
