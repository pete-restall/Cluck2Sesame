#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/PeriodicMonitor.h"

#include "PeriodicMonitorFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Event.c")
TEST_FILE("Platform/Clock/ClockInitialise.c")
TEST_FILE("Platform/Clock/ClockGetSetNow.c")
TEST_FILE("Platform/PeriodicMonitor.c")

void onBeforeTest(void)
{
	periodicMonitorFixtureSetUp();
}

void onAfterTest(void)
{
	periodicMonitorFixtureTearDown();
}

void test_timeChanged_onPublishedWithNonFourMinuteIntervalSchedule_expectMonitoredParamtersSampledEventIsNotPublished(void)
{
	mockOnMonitoredParametersSampled();

	struct Time now =
	{
		.hour = anyByteLessThan(24),
		.second = anyByteLessThan(60)
	};

	for (uint8_t minute = 0; minute < 60; minute++)
	{
		uint8_t offset = minute % 4;
		if (offset == 1)
			continue;

		now.minute = minute;
		publishTimeChanged(&now);
		dispatchAllEvents();
		TEST_ASSERT_EQUAL_UINT8(0, monitoredParametersSampledCalls);
	}
}

void test_timeChanged_onPublishedWithFourMinuteIntervalSchedule_expectMonitoredParamtersSampledEventIsPublished(void)
{
	mockOnMonitoredParametersSampled();

	struct Time now =
	{
		.hour = anyByteLessThan(24),
		.second = anyByteLessThan(60)
	};

	for (uint8_t i = 1, minute = 1; minute < 60; minute += 4, i++)
	{
		now.minute = minute;
		publishTimeChanged(&now);
		dispatchAllEvents();
		TEST_ASSERT_EQUAL_UINT8(i, monitoredParametersSampledCalls);
	}
}

void test_timeChanged_onPublished_expectTmr0lIsUsedAsTimestamp(void)
{
	mockOnMonitoredParametersSampled();

	struct Time now =
	{
		.hour = anyByteLessThan(24),
		.minute = 1,
		.second = anyByteLessThan(60)
	};

	publishTimeChanged(&now);

	TMR0L = anyByte();
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(monitoredParametersSampled);
	TEST_ASSERT_EQUAL_UINT8(TMR0L, monitoredParametersSampled->timestamp);
}

void test_timeChanged_onPublishedWhenRb0IsLow_expectIsVddRegulatedIsFalse(void)
{
	mockOnMonitoredParametersSampled();

	static const struct Time now = { .minute = 1 };
	publishTimeChanged(&now);

	ANSELB = 0;
	TRISB = 0;
	LATB = anyByteWithMaskClear(_PORTB_RB0_MASK);
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(monitoredParametersSampled);
	TEST_ASSERT_FALSE(monitoredParametersSampled->flags.isVddRegulated);
}

void test_timeChanged_onPublishedWhenRb0IsHigh_expectIsVddRegulatedIsTrue(void)
{
	mockOnMonitoredParametersSampled();

	static const struct Time now = { .minute = 1 };
	publishTimeChanged(&now);

	ANSELB = 0;
	TRISB = 0;
	LATB = anyByteWithMaskSet(_PORTB_RB0_MASK);
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(monitoredParametersSampled);
	TEST_ASSERT_TRUE(monitoredParametersSampled->flags.isVddRegulated);
}

void test_timeChanged_onPublished_expectFixedVoltageReferenceIsSampled(void)
{
	mockOnMonitoredParametersSampled();

	struct AdcSample sample =
	{
		.channel = ADC_CHANNEL_ADCFVR,
		.count = 8,
		.result = anyWord()
	};

	stubAdcSampleFor(&sample);

	static const struct Time now = { .minute = 1 };
	publishTimeChanged(&now);
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(monitoredParametersSampled);
	TEST_ASSERT_TRUE(adcSampleCalls >= 1);
	TEST_ASSERT_EQUAL_UINT8(sample.result, monitoredParametersSampled->fvr);
}

void test_timeChanged_onPublished_expectTemperatureSensorIsSampled(void)
{
	mockOnMonitoredParametersSampled();

	struct AdcSample sample =
	{
		.channel = ADC_CHANNEL_TEMPERATURE,
		.count = 8,
		.result = anyWord()
	};

	stubAdcSampleFor(&sample);

	static const struct Time now = { .minute = 1 };
	publishTimeChanged(&now);
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(monitoredParametersSampled);
	TEST_ASSERT_TRUE(adcSampleCalls >= 1);
	TEST_ASSERT_EQUAL_UINT8(sample.result, monitoredParametersSampled->temperature);
}
