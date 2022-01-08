#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <unity.h>

#include "Platform/Temperature.h"

#include "Mock_PeriodicMonitor.h"

#include "../../Fixture.h"
#include "../../NvmSettingsFixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Temperature.c")

#define ROUNDED(t, x) ((t) ((x) < 0 ? (x) - 0.5 : (x) + 0.5))
#define CELSIUS(x) ROUNDED(int16_t, (x) * 10)
#define CELSIUS_HIGH(x) ROUNDED(uint8_t, ((x) - 25) * 10)
#define COEFFICIENT(x) ROUNDED(uint16_t, -(x) * 65536)

static void publishParametersAndAwaitTemperatureSampled(const struct MonitoredParametersSampled *monitoredParametersSampledEventArgs);

static void scenario_onMonitoredParametersSampled_eventWhenPrototypeCalibration_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(
	const char *scenario,
	int16_t expectedCelsius,
	uint16_t sample);

static void scenario_onMonitoredParametersSampled_event_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(
	const char *scenario,
	int16_t expectedCelsius,
	uint16_t sample,
	uint16_t coefficient,
	uint8_t celsiusHigh,
	uint16_t adcHigh);

static void stubNvmSettingsForCalibratedTemperature(uint16_t coefficient, uint8_t celsiusHigh, uint16_t adcHigh);

static struct Event onMonitoredParametersSampledEvent;
static const struct EventSubscription *onMonitoredParametersSampled;
static const struct TemperatureSampled *temperatureSampledEventArgs;

const struct Event eventEmptyArgs = { };

void onBeforeTest(void)
{
	onMonitoredParametersSampled = (const struct EventSubscription *) 0;
	temperatureSampledEventArgs = (const struct TemperatureSampled *) 0;
}

void onAfterTest(void)
{
}

void test_temperatureInitialise_called_expectSubscriptionToMonitoredParametersSampled(void)
{
	temperatureInitialise();
	TEST_ASSERT_NOT_NULL(onMonitoredParametersSampled);
}

void test_onMonitoredParametersSampled_event_expectTemperatureSampledIsPublishedWithSameSample(void)
{
	temperatureInitialise();

	struct MonitoredParametersSampled monitoredParametersSampledEventArgs =
	{
		.temperature = anyWord()
	};

	publishParametersAndAwaitTemperatureSampled(&monitoredParametersSampledEventArgs);
	TEST_ASSERT_EQUAL_UINT16(monitoredParametersSampledEventArgs.temperature, temperatureSampledEventArgs->sample);
}

static void publishParametersAndAwaitTemperatureSampled(const struct MonitoredParametersSampled *monitoredParametersSampledEventArgs)
{
	onMonitoredParametersSampledEvent.args = monitoredParametersSampledEventArgs;
	onMonitoredParametersSampled->handler(&onMonitoredParametersSampledEvent);
	TEST_ASSERT_NOT_NULL_MESSAGE(temperatureSampledEventArgs, "No eventPublish(TemperatureSampled)");
}

void test_onMonitoredParametersSampled_eventAfterInitialise_expectTemperatureSampledIsPublishedWithDeltaSecondsEqualToZero(void)
{
	temperatureInitialise();

	struct MonitoredParametersSampled monitoredParametersSampledEventArgs =
	{
		.timestamp = anyByte()
	};

	publishParametersAndAwaitTemperatureSampled(&monitoredParametersSampledEventArgs);
	TEST_ASSERT_EQUAL_UINT8(0, temperatureSampledEventArgs->deltaSeconds);
}

void test_onMonitoredParametersSampled_eventAfterPreviousEvent_expectTemperatureSampledIsPublishedWithDeltaSecondsEqualToDifference(void)
{
	temperatureInitialise();

	struct MonitoredParametersSampled firstMonitoredParametersSampledEventArgs =
	{
		.timestamp = anyByte()
	};
	publishParametersAndAwaitTemperatureSampled(&firstMonitoredParametersSampledEventArgs);

	struct MonitoredParametersSampled secondMonitoredParametersSampledEventArgs =
	{
		.timestamp = anyByteExcept(firstMonitoredParametersSampledEventArgs.timestamp)
	};
	publishParametersAndAwaitTemperatureSampled(&secondMonitoredParametersSampledEventArgs);

	TEST_ASSERT_EQUAL_UINT8(
		secondMonitoredParametersSampledEventArgs.timestamp - firstMonitoredParametersSampledEventArgs.timestamp,
		temperatureSampledEventArgs->deltaSeconds);
}

void test_onMonitoredParametersSampled_eventAfterInitialise_expectTemperatureSampledIsPublishedWithDeltaCelsiusEqualToZero(void)
{
	stubNvmSettingsForCalibratedTemperature(COEFFICIENT(-0.65), CELSIUS_HIGH(43.2), 0x0100);
	temperatureInitialise();

	struct MonitoredParametersSampled monitoredParametersSampledEventArgs =
	{
		.temperature = anyWord()
	};

	publishParametersAndAwaitTemperatureSampled(&monitoredParametersSampledEventArgs);
	TEST_ASSERT_EQUAL_INT16(0, temperatureSampledEventArgs->deltaCelsius);
}

void test_onMonitoredParametersSampled_eventAfterPreviousEvent_expectTemperatureSampledIsPublishedWithDeltaCelsiusEqualToDifference(void)
{
	stubNvmSettingsForCalibratedTemperature(COEFFICIENT(-0.65), CELSIUS_HIGH(43.2), 0x0100);
	temperatureInitialise();

	struct MonitoredParametersSampled firstMonitoredParametersSampledEventArgs =
	{
		.temperature = anyWord()
	};
	publishParametersAndAwaitTemperatureSampled(&firstMonitoredParametersSampledEventArgs);
	int16_t firstCelsius = temperatureSampledEventArgs->currentCelsius;

	struct MonitoredParametersSampled secondMonitoredParametersSampledEventArgs =
	{
		.temperature = anyWordExcept(firstMonitoredParametersSampledEventArgs.timestamp)
	};
	publishParametersAndAwaitTemperatureSampled(&secondMonitoredParametersSampledEventArgs);
	int16_t secondCelsius = temperatureSampledEventArgs->currentCelsius;

	TEST_ASSERT_EQUAL_INT16(secondCelsius - firstCelsius, temperatureSampledEventArgs->deltaCelsius);
}

void test_onMonitoredParametersSampled_event_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(void)
{
	scenario_onMonitoredParametersSampled_eventWhenPrototypeCalibration_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(
		"1", CELSIUS(21.9), 0x0296);

	scenario_onMonitoredParametersSampled_eventWhenPrototypeCalibration_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(
		"2", CELSIUS(38.4), 0x0281);

	scenario_onMonitoredParametersSampled_eventWhenPrototypeCalibration_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(
		"3", CELSIUS(30.5), 0x028b);

	scenario_onMonitoredParametersSampled_eventWhenPrototypeCalibration_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(
		"4", CELSIUS(0.7), 0x02b1);

	scenario_onMonitoredParametersSampled_eventWhenPrototypeCalibration_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(
		"5", CELSIUS(-0.1), 0x02b2);

	scenario_onMonitoredParametersSampled_eventWhenPrototypeCalibration_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(
		"6", CELSIUS(-261.7), 0x03ff);

	scenario_onMonitoredParametersSampled_eventWhenPrototypeCalibration_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(
		"7", CELSIUS(39.2), 0x0280);

	scenario_onMonitoredParametersSampled_eventWhenPrototypeCalibration_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(
		"8", CELSIUS(542.0), 0x0000);
}

static void scenario_onMonitoredParametersSampled_eventWhenPrototypeCalibration_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(
	const char *scenario,
	int16_t expectedCelsius,
	uint16_t sample)
{
	char subScenario[8] = {0};
	strncpy(subScenario, scenario, 5);
	char *subScenarioIndex = subScenario + strlen(subScenario);
	*(subScenarioIndex++) = '.';

	sample <<= 3;

	*subScenarioIndex = '0';
	scenario_onMonitoredParametersSampled_event_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(
		subScenario, expectedCelsius, sample, COEFFICIENT(-0.7857142857142857), CELSIUS_HIGH(38.4), 0x0281);

	if (sample > 3)
	{
		*subScenarioIndex = '1';
		scenario_onMonitoredParametersSampled_event_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(
			subScenario, expectedCelsius, sample - 3, COEFFICIENT(-0.7857142857142857), CELSIUS_HIGH(38.4), 0x0281);
	}

	*subScenarioIndex = '2';
	scenario_onMonitoredParametersSampled_event_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(
		subScenario, expectedCelsius, sample + 3, COEFFICIENT(-0.7857142857142857), CELSIUS_HIGH(38.4), 0x0281);
}

static void scenario_onMonitoredParametersSampled_event_expectTemperatureSampledIsPublishedWithCalculatedCurrentCelsius(
	const char *scenario,
	int16_t expectedCelsius,
	uint16_t sample,
	uint16_t coefficient,
	uint8_t celsiusHigh,
	uint16_t adcHigh)
{
	stubNvmSettingsForCalibratedTemperature(coefficient, celsiusHigh, adcHigh);
	temperatureInitialise();

	struct MonitoredParametersSampled monitoredParametersSampledEventArgs =
	{
		.temperature = sample
	};

	publishParametersAndAwaitTemperatureSampled(&monitoredParametersSampledEventArgs);
	TEST_ASSERT_EQUAL_INT16_MESSAGE(expectedCelsius, temperatureSampledEventArgs->currentCelsius, scenario);
}

static void stubNvmSettingsForCalibratedTemperature(uint16_t coefficient, uint8_t celsiusHigh, uint16_t adcHigh)
{
	union NvmSettings withCalibratedTemperature =
	{
		.platform =
		{
			.temperature =
			{
				.temperatureHighCelsius = celsiusHigh,
				.temperatureHighAdc = adcHigh,
				.temperatureCoefficient = coefficient
			}
		}
	};

	stubNvmSettings(&withCalibratedTemperature);
}

void eventSubscribe(const struct EventSubscription *subscription)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription, "Null subscription");
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription->handler, "Null handler");
	if (subscription->type == MONITORED_PARAMETERS_SAMPLED)
	{
		onMonitoredParametersSampled = subscription;
		onMonitoredParametersSampledEvent.type = subscription->type;
		onMonitoredParametersSampledEvent.state = subscription->state;
		onMonitoredParametersSampledEvent.args = (void *) 0;
	}
	else
	{
		TEST_FAIL_MESSAGE("Unknown subscription type");
	}
}

void eventPublish(EventType type, const void *args)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(args, "Null args");
	if (type == TEMPERATURE_SAMPLED)
		temperatureSampledEventArgs = (const struct TemperatureSampled *) args;
}
