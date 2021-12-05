#include <xc.h>
#include <stdint.h>

#include "Main.h"
#include "Event.h"
#include "PowerManagement.h"

static void onAllEventsDispatched(const struct Event *event);

void powerManagementInitialise(void)
{
	PMD0 = (uint8_t) ~(_PMD0_SYSCMD_MASK | _PMD0_NVMMD_MASK);
	PMD1 = 0xff;
	PMD2 = 0xff;
	PMD3 = 0xff;
	PMD4 = 0xff;
	PMD5 = 0xff;

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
	eventPublish(WOKEN_FROM_SLEEP, &eventEmptyArgs);

	if (T2CONbits.ON || PIE3bits.RC1IE)
		CPUDOZEbits.IDLEN = 1;
	else
		CPUDOZEbits.IDLEN = 0;

	CPUDOZEbits.DOZEN = 0;

	VREGCONbits.VREGPM = 1;

	asm("sleep");
	asm("nop");
}
