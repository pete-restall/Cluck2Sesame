#include <xc.h>
#include <stdint.h>

#include "../Platform/Event.h"
#include "../Platform/Clock.h"
#include "../Location.h"

#include "SunEvents.h"

static void onLocationChanged(const struct Event *const event);
static void onDateChanged(const struct Event *const event);

void sunEventsInitialise(void)
{
	// TODO: THE NVM_SETTINGS_CHANGED EVENT SHOULD BE HANDLED; PROBABLY RENDERS LOCATION_CHANGED OBSOLETE
	static const struct EventSubscription onLocationChangedSubscription =
	{
		.type = LOCATION_CHANGED,
		.handler = &onLocationChanged,
		.state = (void *) 0
	};

	eventSubscribe(&onLocationChangedSubscription);

	static const struct EventSubscription onDateChangedSubscription =
	{
		.type = DATE_CHANGED,
		.handler = &onDateChanged,
		.state = (void *) 0
	};

	eventSubscribe(&onDateChangedSubscription);
}

static void onLocationChanged(const struct Event *const event)
{
	const struct Location *args =
		((const struct LocationChanged *) event->args)->location;

	sunEventsCalculationContext.inputs.latitudeOffset = args->latitudeOffset;
	sunEventsCalculationContext.inputs.longitudeOffset = args->longitudeOffset;

	sunEventsCalculate();
}

static void onDateChanged(const struct Event *const event)
{
	sunEventsCalculationContext.inputs.dayOfYear =
		((const struct DateChanged *) event->args)->today->dayOfYear;

	sunEventsCalculate();
}
