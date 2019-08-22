#include <xc.h>

#include "Main.h"
#include "Event.h"

void poll(void)
{
	if (!eventDispatchNext())
		eventPublish(ALL_EVENTS_DISPATCHED, &eventEmptyArgs);
}
