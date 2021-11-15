#include <xc.h>
#include <stdint.h>

#include "Event.h"
#include "Clock.h"
#include "Adc.h"
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

	static struct MonitoredParametersSampled eventArgs;
	eventArgs.timestamp = TMR0L;
	eventArgs.flags.isVddRegulated = (PORTBbits.RB0 == 0 ? 0 : 1);

	// TODO: FVR NEEDS STABILISATION TIME OF 60us - FVRRDY BIT IS ALWAYS 1 SO ENABLE IT (PMD0.FVRMD, FVRCON.FVREN), SAMPLE TEMPERATURE FIRST (ALSO REQUIRES ABOUT 25us STABILISATION TIME AFTER ENABLING) AND THEN USE A BUSY LOOP TO MAKE UP THE REST OF THE TIME; ALSO NEEDS TO BE ENABLED (PMD0bits.FVRMD)
	struct AdcSample sample =
	{
		.channel = ADC_CHANNEL_ADCFVR,
		.count = 8
	};

	adcSample(&sample);
	eventArgs.fvr = sample.result;

	sample.channel = ADC_CHANNEL_TEMPERATURE,
	adcSample(&sample);
	eventArgs.temperature = sample.result;

	eventPublish(MONITORED_PARAMETERS_SAMPLED, &eventArgs);
}
