#include <xc.h>

#include "Main.h"
#include "Event.h"

void poll(void)
{
	static const struct AllEventsDispatched emptyEventArgs = { };
	if (!eventDispatchNext())
		eventPublish(ALL_EVENTS_DISPATCHED, &emptyEventArgs);
}
