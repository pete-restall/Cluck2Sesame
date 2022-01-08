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

static void timeChanged_onPublishedNumberOfTimes_expectTimestampIs(uint8_t numberOfTimes, uint8_t expectedTimestamp);

static void publishTimeChangedAndDispatchToEventHandlers(void);

static uint8_t onAdcSampleExpectedPmd0;
static void mockAdcSampleForFixedVoltageReferenceModuleEnabledExpectation(struct AdcSample *sample);

static uint8_t onAdcSampleExpectedFvrcon;
static void mockAdcSampleForFvrconExpectation(struct AdcSample *sample);

static uint8_t spyAdcSampleForChannelsResults[3];
static void spyAdcSampleForChannels(struct AdcSample *sample);

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

void test_timeChanged_onPublishedForFirstTime_expectTimestampIsZero(void)
{
	timeChanged_onPublishedNumberOfTimes_expectTimestampIs(1, 0);
}

static void timeChanged_onPublishedNumberOfTimes_expectTimestampIs(uint8_t numberOfTimes, uint8_t expectedTimestamp)
{
	mockOnMonitoredParametersSampled();

	struct Time now =
	{
		.hour = anyByteLessThan(24),
		.minute = 1,
		.second = anyByteLessThan(60)
	};

	for (uint8_t i = 0; i < numberOfTimes; i++)
	{
		publishTimeChanged(&now);
		TMR0L = anyByte();
		dispatchAllEvents();
	}

	TEST_ASSERT_NOT_NULL(monitoredParametersSampled);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(numberOfTimes, monitoredParametersSampledCalls, "CALLS");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(expectedTimestamp, monitoredParametersSampled->timestamp, "STAMP");
}

void test_timeChanged_onPublishedForSecondTime_expectTimestampIs240(void)
{
	timeChanged_onPublishedNumberOfTimes_expectTimestampIs(2, 240);
}

void test_timeChanged_onPublishedForThirdTime_expectTimestampIs480Modulo256(void)
{
	timeChanged_onPublishedNumberOfTimes_expectTimestampIs(3, 224);
}

void test_timeChanged_onPublishedForThirdTime_expectTimestampIs720Modulo256(void)
{
	timeChanged_onPublishedNumberOfTimes_expectTimestampIs(4, 208);
}

void test_timeChanged_onPublishedForSeventeenthTime_expectTimestampRollsOverToZero(void)
{
	timeChanged_onPublishedNumberOfTimes_expectTimestampIs(17, 0);
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
		.result = anyWord(),
		.flags =
		{
			.vrefIsFvr = 0,
			.acquisitionTimeMultiple = 11
		}
	};

	stubAdcSampleFor(&sample);
	publishTimeChangedAndDispatchToEventHandlers();

	TEST_ASSERT_NOT_NULL(monitoredParametersSampled);
	TEST_ASSERT_TRUE(adcSampleCalls >= 1);
	TEST_ASSERT_EQUAL_UINT8(sample.result, monitoredParametersSampled->fvr);
}

static void publishTimeChangedAndDispatchToEventHandlers(void)
{
	static const struct Time now = { .minute = 1 };
	publishTimeChanged(&now);
	dispatchAllEvents();
}

void test_timeChanged_onPublished_expectTemperatureSensorIsSampledUsingInternalFixedVoltageReference(void)
{
	mockOnMonitoredParametersSampled();

	struct AdcSample sample =
	{
		.channel = ADC_CHANNEL_TEMPERATURE,
		.count = 8,
		.result = anyWord(),
		.flags =
		{
			.vrefIsFvr = 1,
			.acquisitionTimeMultiple = 0
		}
	};

	stubAdcSampleFor(&sample);
	publishTimeChangedAndDispatchToEventHandlers();

	TEST_ASSERT_NOT_NULL(monitoredParametersSampled);
	TEST_ASSERT_TRUE(adcSampleCalls >= 1);
	TEST_ASSERT_EQUAL_UINT8(sample.result, monitoredParametersSampled->temperature);
}

void test_timeChanged_onPublished_expectFixedVoltageReferenceModuleIsDisabledBeforeCompletion(void)
{
	PMD0 = anyByteWithMaskClear(_PMD0_FVRMD_MASK);
	uint8_t originalPmd0 = PMD0;
	publishTimeChangedAndDispatchToEventHandlers();
	TEST_ASSERT_EQUAL_UINT8(originalPmd0 | _PMD0_FVRMD_MASK, PMD0);
}

void test_timeChanged_onPublished_expectFixedVoltageReferenceModuleIsEnabledDuringSampling(void)
{
	PMD0 = anyByteWithMaskSet(_PMD0_FVRMD_MASK);
	onAdcSampleExpectedPmd0 = PMD0 & ~_PMD0_FVRMD_MASK;
	onAdcSample = &mockAdcSampleForFixedVoltageReferenceModuleEnabledExpectation;
	publishTimeChangedAndDispatchToEventHandlers();
}

static void mockAdcSampleForFixedVoltageReferenceModuleEnabledExpectation(struct AdcSample *sample)
{
	TEST_ASSERT_EQUAL_UINT8(onAdcSampleExpectedPmd0, PMD0);
}

void test_timeChanged_onPublished_expectFixedVoltageReferenceIs2048mvAndTemperatureRangeIsHighDuringSampling(void)
{
	PMD0 = anyByteWithMaskSet(_PMD0_FVRMD_MASK);
	FVRCON = anyByte();
	onAdcSampleExpectedFvrcon = _FVRCON_ADFVR1_MASK | _FVRCON_FVRRDY_MASK | _FVRCON_FVREN_MASK | _FVRCON_TSRNG_MASK | _FVRCON_TSEN_MASK;
	onAdcSample = &mockAdcSampleForFvrconExpectation;
	publishTimeChangedAndDispatchToEventHandlers();
}

static void mockAdcSampleForFvrconExpectation(struct AdcSample *sample)
{
	TEST_ASSERT_EQUAL_UINT8(onAdcSampleExpectedFvrcon, FVRCON);
}

void test_timeChanged_onPublished_expectSamplingIsFixedVoltageReferenceThenTemperatureToAllowForStabilisationPeriods(void)
{
	spyAdcSampleForChannelsResults[0] = ADC_CHANNEL_CMPFVR;
	spyAdcSampleForChannelsResults[1] = ADC_CHANNEL_CMPFVR;
	onAdcSample = &spyAdcSampleForChannels;
	publishTimeChangedAndDispatchToEventHandlers();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(ADC_CHANNEL_ADCFVR, spyAdcSampleForChannelsResults[0], "FVR");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(ADC_CHANNEL_TEMPERATURE, spyAdcSampleForChannelsResults[1], "TEMP");
}

static void spyAdcSampleForChannels(struct AdcSample *sample)
{
	if (sample && adcSampleCalls < sizeof(spyAdcSampleForChannelsResults))
		spyAdcSampleForChannelsResults[adcSampleCalls] = sample->channel;

	adcSampleCalls++;
}
