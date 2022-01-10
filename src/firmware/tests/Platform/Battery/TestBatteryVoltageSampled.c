#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Mock_PeriodicMonitor.h"
#include "Platform/Battery.h"

#include "BatteryFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Battery.c")

static void onBatteryVoltageSampled(const struct Event *event);
static uint16_t anyValidFvrSample(void);

static const struct BatteryVoltageSampled *batteryVoltageSampledEventArgs;

const struct Event eventEmptyArgs = { };

void onBeforeTest(void)
{
	batteryFixtureSetUp();
	eventInitialise();

	static const struct EventSubscription onBatteryVoltageSampledSubscription =
	{
		.type = BATTERY_VOLTAGE_SAMPLED,
		.handler = &onBatteryVoltageSampled,
		.state = (void *) 0
	};

	eventSubscribe(&onBatteryVoltageSampledSubscription);
	batteryVoltageSampledEventArgs = (const struct BatteryVoltageSampled *) 0;
}

static void onBatteryVoltageSampled(const struct Event *event)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(event, "Null event !");
	batteryVoltageSampledEventArgs = (const struct BatteryVoltageSampled *) event->args;
	TEST_ASSERT_NOT_NULL_MESSAGE(batteryVoltageSampledEventArgs, "Null event args !");
}

void onAfterTest(void)
{
	batteryFixtureTearDown();
}

void test_batteryInitialise_expectNoBatteryVoltageSampledEventIsPublished(void)
{
	batteryInitialise();
	dispatchAllEvents();
	TEST_ASSERT_NULL(batteryVoltageSampledEventArgs);
}

void test_onMonitoredParametersSampled_eventPublishedWithIdVddRegulatedFlagSet_expectNoBatteryVoltageSampledEventIsPublished(void)
{
	batteryInitialise();
	struct MonitoredParametersSampled eventArgs =
	{
		.fvr = anyValidFvrSample(),
		.flags = { .isVddRegulated = 1 }
	};
	eventPublish(MONITORED_PARAMETERS_SAMPLED, &eventArgs);
	dispatchAllEvents();
	TEST_ASSERT_NULL(batteryVoltageSampledEventArgs);
}

static uint16_t anyValidFvrSample(void)
{
	return FVR_MINIMUM_VALID_SAMPLE + anyWordLessThan(FVR_MAXIMUM_VALID_SAMPLE - FVR_MINIMUM_VALID_SAMPLE + 1);
}

void test_onMonitoredParametersSampled_eventPublishedWithIsVddRegulatedFlagClearAndLessThanMinimumFvrSample_expectNoBatteryVoltageSampledEventIsPublished(void)
{
	uint16_t fvrMillivolts = stubAnyDiaFvra2xMillivolts();
	stubDiaFvra2xMillivolts(fvrMillivolts);
	batteryInitialise();
	for (uint16_t i = 1; i < 10; i++)
	{
		struct MonitoredParametersSampled eventArgs =
		{
			.fvr = FVR_MINIMUM_VALID_SAMPLE - i,
			.flags = { .isVddRegulated = 0 }
		};
		eventPublish(MONITORED_PARAMETERS_SAMPLED, &eventArgs);
		dispatchAllEvents();
		TEST_ASSERT_NULL(batteryVoltageSampledEventArgs);
	}
}

void test_onMonitoredParametersSampled_eventPublishedWithIsVddRegulatedFlagClearAndMoreThanMaximumFvrSample_expectNoBatteryVoltageSampledEventIsPublished(void)
{
	uint16_t fvrMillivolts = stubAnyDiaFvra2xMillivolts();
	stubDiaFvra2xMillivolts(fvrMillivolts);
	batteryInitialise();
	for (uint16_t i = 1; i < 10; i++)
	{
		struct MonitoredParametersSampled eventArgs =
		{
			.fvr = FVR_MAXIMUM_VALID_SAMPLE + i,
			.flags = { .isVddRegulated = 0 }
		};
		eventPublish(MONITORED_PARAMETERS_SAMPLED, &eventArgs);
		dispatchAllEvents();
		TEST_ASSERT_NULL(batteryVoltageSampledEventArgs);
	}
}

void test_onMonitoredParametersSampled_eventPublishedWithIsVddRegulatedFlagClear_expectPublishedEventHasFvrSample(void)
{
	stubAnyDiaFvra2xMillivolts();
	batteryInitialise();
	struct MonitoredParametersSampled eventArgs =
	{
		.fvr = anyValidFvrSample(),
		.flags = { .isVddRegulated = 0 }
	};
	eventPublish(MONITORED_PARAMETERS_SAMPLED, &eventArgs);
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(batteryVoltageSampledEventArgs);
	TEST_ASSERT_EQUAL_UINT16(eventArgs.fvr, batteryVoltageSampledEventArgs->sample);
}

void test_onMonitoredParametersSampled_eventPublishedWithIsVddRegulatedFlagClearAndMinimumFvrSample_expectPublishedEventHasCalculatedVddMillivolts(void)
{
	uint16_t fvrMillivolts = stubAnyDiaFvra2xMillivolts();
	uint32_t vddMillivolts = (uint32_t) fvrMillivolts * 8192 / FVR_MINIMUM_VALID_SAMPLE;
	stubDiaFvra2xMillivolts(fvrMillivolts);
	batteryInitialise();
	struct MonitoredParametersSampled eventArgs =
	{
		.fvr = FVR_MINIMUM_VALID_SAMPLE,
		.flags = { .isVddRegulated = 0 }
	};
	eventPublish(MONITORED_PARAMETERS_SAMPLED, &eventArgs);
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(batteryVoltageSampledEventArgs);
	TEST_ASSERT_EQUAL_UINT16(vddMillivolts, batteryVoltageSampledEventArgs->millivolts);
}

void test_onMonitoredParametersSampled_eventPublishedWithIsVddRegulatedFlagClearAndMaximumFvrSample_expectPublishedEventHasCalculatedVddMillivolts(void)
{
	uint16_t fvrMillivolts = stubAnyDiaFvra2xMillivolts();
	uint32_t vddMillivolts = (uint32_t) fvrMillivolts * 8192 / FVR_MAXIMUM_VALID_SAMPLE;
	stubDiaFvra2xMillivolts(fvrMillivolts);
	batteryInitialise();
	struct MonitoredParametersSampled eventArgs =
	{
		.fvr = FVR_MAXIMUM_VALID_SAMPLE,
		.flags = { .isVddRegulated = 0 }
	};
	eventPublish(MONITORED_PARAMETERS_SAMPLED, &eventArgs);
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(batteryVoltageSampledEventArgs);
	TEST_ASSERT_EQUAL_UINT16(vddMillivolts, batteryVoltageSampledEventArgs->millivolts);
}

void test_onMonitoredParametersSampled_eventPublishedWithIsVddRegulatedFlagClearAndValidFvrSample_expectPublishedEventHasCalculatedVddMillivolts(void)
{
	uint16_t fvrMillivolts = stubAnyDiaFvra2xMillivolts();
	stubDiaFvra2xMillivolts(fvrMillivolts);
	batteryInitialise();
	struct MonitoredParametersSampled eventArgs =
	{
		.fvr = anyValidFvrSample(),
		.flags = { .isVddRegulated = 0 }
	};
	eventPublish(MONITORED_PARAMETERS_SAMPLED, &eventArgs);
	dispatchAllEvents();
	uint32_t vddMillivolts = (uint32_t) fvrMillivolts * 8192 / eventArgs.fvr;
	TEST_ASSERT_NOT_NULL(batteryVoltageSampledEventArgs);
	TEST_ASSERT_EQUAL_UINT16(vddMillivolts, batteryVoltageSampledEventArgs->millivolts);
}
