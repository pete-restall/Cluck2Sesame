#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/Clock.h"
#include "Platform/Adc.h"

#include "PeriodicMonitorFixture.h"
#include "../../NonDeterminism.h"

static void onMonitoredParametersSampled(const struct Event *event);
static void onTimeChanged(const struct Event *event);
static void stubAdcSample(struct AdcSample *sample);

static uint8_t stubCallSequence;

const struct TimeChanged *timeChanged;
uint8_t timeChangedCalls;
uint8_t timeChangedSequence;

const struct MonitoredParametersSampled *monitoredParametersSampled;
uint8_t monitoredParametersSampledCalls;
uint8_t monitoredParametersSampledSequence;

static struct AdcSample *expectedAdcSampleArg;
uint8_t adcSampleCalls;
uint8_t adcSampleSequence;
AdcSampleCallback onAdcSample;

static void buggyCompilerWorkaround(void)
{
	onAdcSample = _OMNITARGET;
}

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

	expectedAdcSampleArg = (struct AdcSample *) 0;
	adcSampleCalls = 0;
	adcSampleSequence = 0;
	onAdcSample = &stubAdcSample;
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
	static struct TimeChanged eventArgs;
	eventArgs.now = now;
	eventPublish(TIME_CHANGED, &eventArgs);
}

void stubAdcSampleFor(struct AdcSample *sample)
{
	expectedAdcSampleArg = sample;
}

void adcSample(struct AdcSample *sample)
{
	if (onAdcSample)
		onAdcSample(sample);
}

static void stubAdcSample(struct AdcSample *sample)
{
	adcSampleCalls++;
	adcSampleSequence = stubCallSequence++;
	if (!expectedAdcSampleArg || !sample)
		return;

	if (sample->channel != expectedAdcSampleArg->channel)
		return;

	if (sample->count != expectedAdcSampleArg->count)
		return;

	if (sample->flags.all != expectedAdcSampleArg->flags.all)
		return;

	sample->result = expectedAdcSampleArg->result;
}
