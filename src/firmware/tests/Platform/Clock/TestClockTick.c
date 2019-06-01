#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/PowerManagement.h"
#include "Platform/Clock.h"

#include "ClockFixture.h"
#include "ClockGetSetNowFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

TEST_FILE("Platform/Clock/ClockInitialise.c")
TEST_FILE("Platform/Clock/ClockGetSetNow.c")

static void tickWithoutSettingInterruptFlag(void);

void onBeforeTest(void)
{
	clockFixtureSetUp();
	clockGetSetNowFixtureSetUp();
}

void onAfterTest(void)
{
	clockFixtureTearDown();
}

void test_tick_onWokenFromSleepWhenTimerInterruptFlagIsClear_expectTimeIsNotIncremented(void)
{
	stubAnyDateTime();
	struct DateAndTimeGet before;
	clockGetNowGmt(&before);
	dispatchAllEvents();

	PIR0bits.TMR0IF = 0;
	tickWithoutSettingInterruptFlag();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	assertEqualDateTime(&before, &now);
}

static void tickWithoutSettingInterruptFlag(void)
{
	publishWokenFromSleep();
	dispatchAllEvents();
}

void test_tick_onWokenFromSleepWhenTimerInterruptFlagIsSet_expectTimeIsIncremented(void)
{
	stubAnyDateTimeWithHourAndMinute(anyByteLessThan(23), anyByteLessThan(59));
	struct DateAndTimeGet before;
	clockGetNowGmt(&before);
	dispatchAllEvents();

	PIR0bits.TMR0IF = 1;
	tickWithoutSettingInterruptFlag();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	assertEqualDateTimeExceptMinute(&before, &now);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(before.time.minute + 1, now.time.minute, "MM");
}

void test_tick_onWokenFromSleepWhenTimerInterruptFlagIsSet_expectTimerInterruptFlagIsCleared(void)
{
	PIR0 = anyByteWithMaskSet(_PIR0_TMR0IF_MASK);
	uint8_t originalPir0 = PIR0;
	tickWithoutSettingInterruptFlag();
	TEST_ASSERT_EQUAL_UINT8(originalPir0 & ~_PIR0_TMR0IF_MASK, PIR0);
}

void test_tick_happensWhenTmr0hIsNotOneMinute_expectTmr0hIsResetToOneMinute(void)
{
	TMR0H = anyByteExcept(59);
	tick();
	TEST_ASSERT_EQUAL_UINT8(59, TMR0H);
}

void test_tick_happens_expectTimeChangedEventIsPublished(void)
{
	stubAnyDateTime();
	dispatchAllEvents();
	mockOnTimeChanged();
	tick();

	struct DateAndTimeGet now;
	clockGetNowGmt(&now);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, timeChangedCalls, "Num Events");
	TEST_ASSERT_NOT_NULL_MESSAGE(timeChanged, "Event");

	TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
		&now.time,
		timeChanged->now,
		sizeof(struct Time),
		"Time");
}

void test_tick_happensWhenTicksToNextDay_expectDateChangedEventIsPublished(void)
{
	stubAnyDateTimeWithHourAndMinute(23, 59);
	dispatchAllEvents();
	mockOnDateChanged();
	tick();

	struct DateAndTimeGet now;
	clockGetNowGmt(&now);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, dateChangedCalls, "Num Events");
	TEST_ASSERT_NOT_NULL_MESSAGE(dateChanged, "Event");

	TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
		&now.date,
		dateChanged->today,
		sizeof(struct DateWithFlags),
		"Date");
}

void test_tick_happensWhenDoesNotTickToNextDay_expectNoDateChangedEventIsPublished(void)
{
	stubAnyDateTimeWithHourAndMinute(23, anyByteLessThan(59));
	dispatchAllEvents();
	mockOnDateChanged();
	tick();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, dateChangedCalls, "Num Events");
}

void test_tick_happensWhenTicksToNextDay_expectDateChangedEventIsPublishedBeforeTimeChanged(void)
{
	stubAnyDateTimeWithHourAndMinute(23, 59);
	dispatchAllEvents();
	mockOnDateChanged();
	mockOnTimeChanged();
	tick();
	TEST_ASSERT_TRUE(dateChangedSequence < timeChangedSequence);
}

void test_tick_happensWhenLeapYearTicksToNextYear_expectIsLeapYearFlagIsClear(void)
{
	struct DateAndTimeSet now =
	{
		.date =
		{
			.year = anyLeapYear(),
			.month = 12,
			.day = 31
		},
		.time =
		{
			.hour = 23,
			.minute = 59
		}
	};

	clockSetNowGmt(&now);
	dispatchAllEvents();

	mockOnDateChanged();
	tick();
	TEST_ASSERT_NOT_NULL(dateChanged);
	TEST_ASSERT_FALSE(dateChanged->today->flags.isLeapYear);
}

void test_tick_happensWhenNonLeapYearTicksToNextYear_expectIsLeapYearFlagIsSet(void)
{
	struct DateAndTimeSet now =
	{
		.date =
		{
			.year = anyLeapYear() + 3,
			.month = 12,
			.day = 31
		},
		.time =
		{
			.hour = 23,
			.minute = 59
		}
	};

	clockSetNowGmt(&now);
	dispatchAllEvents();

	mockOnDateChanged();
	tick();
	TEST_ASSERT_NOT_NULL(dateChanged);
	TEST_ASSERT_TRUE(dateChanged->today->flags.isLeapYear);
}
