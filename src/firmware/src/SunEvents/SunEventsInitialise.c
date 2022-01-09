#include <xc.h>
#include <stdint.h>

#include "../Platform/Event.h"
#include "../Platform/Clock.h"
#include "../Platform/NvmSettings.h"

#include "../Location.h"

#include "SunEvents.h"

static void onLocationChanged(const struct Event *event);
static void onDateChanged(const struct Event *event);

void sunEventsInitialise(void)
{
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

	sunEventsCalculationContext.inputs.latitudeOffset = nvmSettings.application.location.latitudeOffset;
	sunEventsCalculationContext.inputs.longitudeOffset = nvmSettings.application.location.longitudeOffset;
}

static void onLocationChanged(const struct Event *event)
{
	const struct Location *args = ((const struct LocationChanged *) event->args)->location;
	sunEventsCalculationContext.inputs.latitudeOffset = args->latitudeOffset;
	sunEventsCalculationContext.inputs.longitudeOffset = args->longitudeOffset;
	sunEventsCalculate();
}

static void onDateChanged(const struct Event *event)
{
	sunEventsCalculationContext.inputs.dayOfYear = ((const struct DateChanged *) event->args)->today->dayOfYear;
	sunEventsCalculate();
}
