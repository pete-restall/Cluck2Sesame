#include <xc.h>
#include <stdint.h>

#include "../Event.h"
#include "../Clock.h"

#include "SunEvents.h"

static void onDateChanged(const struct Event *const event);

extern const __rom void *const sunriseLookupTable;
extern const __rom void *const sunsetLookupTable;

void sunEventsInitialise(void)
{
	static const struct EventSubscription onDateChangedSubscription =
	{
		.type = DATE_CHANGED,
		.handler = &onDateChanged,
		.state = (void *) 0
	};

	// TODO: THIS IS BIG CODE, SO ENCAPSULATE AN ENTRY LOOKUP INTO A FUNCTION...
	// TODO: THIS USELESS 'a + b' CAN BE REMOVED ONCE THE LOOKUPS ARE BEING USED PROPERLY; JUST A PLACEHOLDER TO PREVENT THE COMPILER REMOVING THEM...
	uint8_t a = ((const uint8_t *) sunriseLookupTable)[0];
	uint8_t b = ((const uint8_t *) sunsetLookupTable)[0];
	uint8_t c = a + b;

	eventSubscribe(&onDateChangedSubscription);
}

static void onDateChanged(const struct Event *const event)
{
	static const struct SunEventsChanged eventArgs = { /* TODO */ };
	eventPublish(SUN_EVENTS_CHANGED, &eventArgs);
}
