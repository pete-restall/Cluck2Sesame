#include <xc.h>
#include <stdint.h>

#include "../Platform/Event.h"
#include "../Platform/Buttons.h"

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

	uiState.flags.all = 0;
}
