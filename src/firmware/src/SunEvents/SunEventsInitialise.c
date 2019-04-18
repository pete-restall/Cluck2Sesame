#include <xc.h>
#include <stdint.h>

#include "../Event.h"
#include "../Clock.h"

#include "SunEvents.h"

static void onDateChanged(const struct Event *const event);

void sunEventsInitialise(void)
{
	static const struct EventSubscription onDateChangedSubscription =
	{
		.type = DATE_CHANGED,
		.handler = &onDateChanged,
		.state = (void *) 0
	};

	eventSubscribe(&onDateChangedSubscription);
}

static void onDateChanged(const struct Event *const event)
{
	sunEventsCalculationContext.inputs.dayOfYear =
		((const struct DateChanged *) event->args)->today->dayOfYear;

	sunEventsCalculate();
}
