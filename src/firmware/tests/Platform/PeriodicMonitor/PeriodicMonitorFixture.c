#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/Clock.h"

#include "PeriodicMonitorFixture.h"
#include "../../NonDeterminism.h"

static void onMonitoredParametersSampled(const struct Event *event);
static void onTimeChanged(const struct Event *event);

static uint8_t stubCallSequence;

const struct TimeChanged *timeChanged;
uint8_t timeChangedCalls;
uint8_t timeChangedSequence;

const struct MonitoredParametersSampled *monitoredParametersSampled;
uint8_t monitoredParametersSampledCalls;
uint8_t monitoredParametersSampledSequence;

void periodicMonitorFixtureSetUp(void)
{
	eventInitialise();
	periodicMonitorInitialise();

	stubCallSequence = 1;

	timeChanged = (const struct TimeChanged *) 0;
	timeChangedCalls = 0;
	timeChangedSequence = 0;

	monitoredParametersSampled = (const struct MonitoredParametersSampled *) 0;
	monitoredParametersSampledCalls = 0;
	monitoredParametersSampledSequence = 0;
}

void periodicMonitorFixtureTearDown(void)
{
}

void mockOnMonitoredParametersSampled(void)
{
	static const struct EventSubscription onMonitoredParametersSampledSubscription =
	{
		.type = MONITORED_PARAMETERS_SAMPLED,
		.handler = &onMonitoredParametersSampled
	};

	eventSubscribe(&onMonitoredParametersSampledSubscription);
}

static void onMonitoredParametersSampled(const struct Event *event)
{
	monitoredParametersSampled = (const struct MonitoredParametersSampled *) event->args;
	monitoredParametersSampledCalls++;
	monitoredParametersSampledSequence = stubCallSequence++;
}

void publishTimeChanged(const struct Time *now)
{
	struct TimeChanged eventArgs = { .now = now };
	eventPublish(TIME_CHANGED, &eventArgs);
}
