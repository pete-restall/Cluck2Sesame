#include <xc.h>
#include <stdint.h>

#include "Event.h"
#include "PowerManagement.h"
#include "NearScheduler.h"

#include "Buttons.h"

#define BUTTON1_PORT_MASK _PORTA_RA0_MASK
#define BUTTON2_PORT_MASK _PORTA_RA1_MASK
#define BUTTON_PORT_MASK (BUTTON1_PORT_MASK | BUTTON2_PORT_MASK)
#define BUTTON_STATE_NOW (PORTA & BUTTON_PORT_MASK)

static void onWokenFromSleep(const struct Event *event);
static void onButtonsStoppedBouncing(void *state);

static uint8_t buttonStateSampled;

void buttonsInitialise(void)
{
	PMD0bits.IOCMD = 0;
	ANSELA &= ~BUTTON_PORT_MASK;
	PIE0bits.IOCIE = 1;
	IOCAN |= BUTTON_PORT_MASK;
	IOCAP |= BUTTON_PORT_MASK;
	buttonStateSampled = BUTTON_STATE_NOW;

	static const struct EventSubscription onWokenFromSleepSubscription =
	{
		.type = WOKEN_FROM_SLEEP,
		.handler = &onWokenFromSleep,
		.state = (void *) 0
	};

	eventSubscribe(&onWokenFromSleepSubscription);
}

static void onWokenFromSleep(const struct Event *event)
{
	if (!PIR0bits.IOCIF)
		return;

	IOCAF = 0;

	static const struct NearSchedule onButtonsStoppedBouncingSchedule =
	{
		.ticks = MS_TO_TICKS(32),
		.handler = &onButtonsStoppedBouncing
	};

	nearSchedulerAddOrUpdate(&onButtonsStoppedBouncingSchedule);
}

static void onButtonsStoppedBouncing(void *state)
{
	uint8_t buttonsChanged = buttonStateSampled ^ BUTTON_STATE_NOW;
	if (!buttonsChanged)
		return;

	if (buttonsChanged & BUTTON1_PORT_MASK)
	{
		static const struct ButtonsPressed button1EventArgs = { .mask = 0x01 };
		if ((buttonStateSampled & BUTTON1_PORT_MASK) == 0)
			eventPublish(BUTTONS_RELEASED, &button1EventArgs);
		else
			eventPublish(BUTTONS_PRESSED, &button1EventArgs);
	}

	if (buttonsChanged & BUTTON2_PORT_MASK)
	{
		static const struct ButtonsPressed button2EventArgs = { .mask = 0x02 };
		if ((buttonStateSampled & BUTTON2_PORT_MASK) == 0)
			eventPublish(BUTTONS_RELEASED, &button2EventArgs);
		else
			eventPublish(BUTTONS_PRESSED, &button2EventArgs);
	}

	buttonStateSampled ^= buttonsChanged;
}
