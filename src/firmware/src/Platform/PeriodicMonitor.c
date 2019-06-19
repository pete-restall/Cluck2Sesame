#include <xc.h>
#include <stdint.h>

#include "Event.h"
#include "Clock.h"
#include "PeriodicMonitor.h"

static void onTimeChanged(const struct Event *event);

void periodicMonitorInitialise(void)
{
	static const struct EventSubscription onTimeChangedSubscription =
	{
		.type = TIME_CHANGED,
		.handler = &onTimeChanged,
		.state = (void *) 0
	};

	eventSubscribe(&onTimeChangedSubscription);
}

static void onTimeChanged(const struct Event *event)
{
	const struct TimeChanged *args = (const struct TimeChanged *) event->args;
	if ((args->now->minute & 0x03) != 1)
		return;

	static const struct MonitoredParametersSampled eventArgs = { };
	eventPublish(MONITORED_PARAMETERS_SAMPLED, &eventArgs);
}
