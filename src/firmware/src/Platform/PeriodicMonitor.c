#include <xc.h>
#include <stdint.h>

#define __PERIODICMONITOR_EXPOSE_INTERNALS

#include "Event.h"
#include "Clock.h"
#include "Adc.h"
#include "PeriodicMonitor.h"

static void onTimeChanged(const struct Event *event);

static uint8_t nextTimestamp;

void periodicMonitorInitialise(void)
{
	static const struct EventSubscription onTimeChangedSubscription =
	{
		.type = TIME_CHANGED,
		.handler = &onTimeChanged,
		.state = (void *) 0
	};

	eventSubscribe(&onTimeChangedSubscription);

	nextTimestamp = 0;
}

static void onTimeChanged(const struct Event *event)
{
	const struct TimeChanged *args = (const struct TimeChanged *) event->args;
	if ((args->now->minute & 0x03) != 1)
		return;

	static struct MonitoredParametersSampled eventArgs;
	periodicMonitorSampleNow(&eventArgs);
	eventPublish(MONITORED_PARAMETERS_SAMPLED, &eventArgs);
}

void periodicMonitorSampleNow(struct MonitoredParametersSampled *eventArgs)
{
	if (!eventArgs)
		return;

	eventArgs->timestamp = nextTimestamp;
	eventArgs->flags.isVddRegulated = (PORTBbits.RB0 == 0 ? 0 : 1);
	nextTimestamp += 240;

	PMD0bits.FVRMD = 0;
	FVRCON = _FVRCON_ADFVR1_MASK | _FVRCON_FVREN_MASK | _FVRCON_TSRNG_MASK | _FVRCON_TSEN_MASK;

	struct AdcSample sample =
	{
		.channel = ADC_CHANNEL_ADCFVR,
		.count = 8,
		.flags =
		{
			.vrefIsFvr = 0,
			.acquisitionTimeMultiple = 11
		}
	};

	adcSample(&sample);
	eventArgs->fvr = sample.result;

	sample.channel = ADC_CHANNEL_TEMPERATURE,
	sample.flags.vrefIsFvr = 1;
	sample.flags.acquisitionTimeMultiple = 0;
	adcSample(&sample);
	eventArgs->temperature = sample.result;

	PMD0bits.FVRMD = 1;
}
