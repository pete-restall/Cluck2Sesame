#include <xc.h>

#include "Main.h"
#include "Event.h"
#include "PowerManagement.h"

static void onAllEventsDispatched(const struct Event *event);

// TODO: DISABLE ALL UNNECESSARY PERIPHERALS VIA THE PMD REGISTERS !
void powerManagementInitialise(void)
{
	static const struct EventSubscription onAllEventsDispatchedSubscription =
	{
		.type = ALL_EVENTS_DISPATCHED,
		.handler = &onAllEventsDispatched,
		.state = (void *) 0
	};

	eventSubscribe(&onAllEventsDispatchedSubscription);
	INTCONbits.PEIE = 1;
}

static void onAllEventsDispatched(const struct Event *event)
{
	static const struct WokenFromSleep noEventArgs = { };
	eventPublish(WOKEN_FROM_SLEEP, &noEventArgs);

	VREGCONbits.VREGPM = 1;
	asm("sleep");
	asm("nop");
}
