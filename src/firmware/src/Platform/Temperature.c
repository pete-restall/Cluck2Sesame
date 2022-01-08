#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "Event.h"
#include "NvmSettings.h"
#include "PeriodicMonitor.h"
#include "Temperature.h"

static void onMonitoredParametersSampled(const struct Event *event);

static bool hasPreviousSample;
static uint8_t previousSampleTimestamp;
static int16_t previousSampleCelsius;
static int32_t scaledTemperatureCoefficient;

void temperatureInitialise(void)
{
	static const struct EventSubscription onMonitoredParametersSampledSubscription =
	{
		.type = MONITORED_PARAMETERS_SAMPLED,
		.handler = &onMonitoredParametersSampled,
		.state = (void *) 0
	};

	eventSubscribe(&onMonitoredParametersSampledSubscription);

	hasPreviousSample = false;
	scaledTemperatureCoefficient = nvmSettings.platform.temperature.temperatureCoefficient;
	scaledTemperatureCoefficient *= -10;
}

static void onMonitoredParametersSampled(const struct Event *event)
{
	const struct MonitoredParametersSampled *args = (const struct MonitoredParametersSampled *) event->args;

	static struct TemperatureSampled eventArgs;
	eventArgs.sample = args->temperature;

	int32_t celsius = scaledTemperatureCoefficient;
	celsius *= (int16_t) (((eventArgs.sample + 4) >> 3) - nvmSettings.platform.temperature.temperatureHighAdc);
	celsius += (((int32_t) nvmSettings.platform.temperature.temperatureHighCelsius + 25 * 10) << 16) + 32768;
	eventArgs.currentCelsius = (int16_t) (celsius >> 16);

	if (!hasPreviousSample)
	{
		eventArgs.deltaSeconds = 0;
		eventArgs.deltaCelsius = 0;
		hasPreviousSample = true;
	}
	else
	{
		eventArgs.deltaSeconds = args->timestamp - previousSampleTimestamp;
		eventArgs.deltaCelsius = eventArgs.currentCelsius - previousSampleCelsius;
	}

	previousSampleTimestamp = args->timestamp;
	previousSampleCelsius = eventArgs.currentCelsius;

	eventPublish(TEMPERATURE_SAMPLED, &eventArgs);
}
