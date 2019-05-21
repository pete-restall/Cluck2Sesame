#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Event.h"
#include "PowerManagement.h"
#include "Clock.h"

#include "ClockFixture.h"
#include "../NonDeterminism.h"

TEST_FILE("Clock/ClockInitialise.c")
TEST_FILE("Clock/ClockGetSetNow.c")

static void tick(void);

void setUp(void)
{
	clockFixtureSetUp();
	eventInitialise();
	clockInitialise();
	T0CON0bits.T0EN = 0;
	T0CON1bits.T0CKPS = 0;
}

void tearDown(void)
{
	clockFixtureTearDown();
}

void test_tick_onWokenFromSleepWhenTimerInterruptFlagIsClear_expectTimeIsNotIncremented(void)
{
	stubAnyDateTime();
	struct DateAndTimeGet before;
	clockGetNowGmt(&before);

	PIR0bits.TMR0IF = 0;
	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	assertEqualDateTime(&before, &now);
}

static void tick(void)
{
	publishWokenFromSleep();
	dispatchAllEvents();
}

void test_tick_onWokenFromSleepWhenTimerInterruptFlagIsSet_expectTimeIsIncremented(void)
{
	stubAnyDateTimeWithHourAndMinute(anyByteLessThan(23), anyByteLessThan(59));
	struct DateAndTimeGet before;
	clockGetNowGmt(&before);

	PIR0bits.TMR0IF = 1;
	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	assertEqualDateTimeExceptMinute(&before, &now);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(before.time.minute + 1, now.time.minute, "MM");
}

void test_tick_onWokenFromSleepWhenTimerInterruptFlagIsSet_expectTimerInterruptFlagIsCleared(void)
{
	PIR0 = anyByteWithMaskSet(_PIR0_TMR0IF_MASK);
	uint8_t originalPir0 = PIR0;
	tick();
	TEST_ASSERT_EQUAL_UINT8(originalPir0 & ~_PIR0_TMR0IF_MASK, PIR0);
}

void test_tick_calledWhenTmr0hIsNotOneMinute_expectTmr0hIsResetToOneMinute(void)
{
	TMR0H = anyByteExcept(59);
	PIR0bits.TMR0IF = 1;
	tick();
	TEST_ASSERT_EQUAL_UINT8(59, TMR0H);
}

void test_tick_called_expectTimeChangedEventIsPublished(void)
{
	stubAnyDateTime();
	mockOnTimeChanged();
	PIR0bits.TMR0IF = 1;
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

void test_tick_calledWhenTicksToNextDay_expectDateChangedEventIsPublished(void)
{
	stubAnyDateTimeWithHourAndMinute(23, 59);
	mockOnDateChanged();
	PIR0bits.TMR0IF = 1;
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

void test_tick_calledWhenDoesNotTickToNextDay_expectNoDateChangedEventIsPublished(void)
{
	stubAnyDateTimeWithHourAndMinute(23, anyByteLessThan(59));
	mockOnDateChanged();
	PIR0bits.TMR0IF = 1;
	tick();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, dateChangedCalls, "Num Events");
}

void test_tick_calledWhenTicksToNextDay_expectDateChangedEventIsPublishedBeforeTimeChanged(void)
{
	stubAnyDateTimeWithHourAndMinute(23, 59);
	mockOnDateChanged();
	mockOnTimeChanged();
	PIR0bits.TMR0IF = 1;
	tick();
	TEST_ASSERT_TRUE(dateChangedSequence < timeChangedSequence);
}
