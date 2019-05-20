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
static void publishWokenFromSleep(void);
static void dispatchAllEvents(void);
static void mockOnTimeChanged(void);
static void onTimeChanged(const struct Event *const event);
static void mockOnDateChanged(void);
static void onDateChanged(const struct Event *const event);

static uint8_t stubCallSequence;

static const struct DateChanged *dateChanged;
static uint8_t dateChangedCalls;
static uint8_t dateChangedSequence;

static const struct TimeChanged *timeChanged;
static uint8_t timeChangedCalls;
static uint8_t timeChangedSequence;

void setUp(void)
{
	eventInitialise();
	clockInitialise();
	T0CON0bits.T0EN = 0;
	T0CON1bits.T0CKPS = 0;

	stubCallSequence = 1;
	dateChanged = (const struct DateChanged *) 0;
	dateChangedCalls = dateChangedSequence = 0;
	timeChanged = (const struct TimeChanged *) 0;
	timeChangedCalls = timeChangedSequence = 0;
}

void tearDown(void)
{
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

static void publishWokenFromSleep(void)
{
	static const struct WokenFromSleep emptyArgs = { };
	eventPublish(WOKEN_FROM_SLEEP, &emptyArgs);
}

static void dispatchAllEvents(void)
{
	while (eventDispatchNext())
		;;
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

static void mockOnTimeChanged(void)
{
	static const struct EventSubscription onTimeChangedSubscription =
	{
		.type = TIME_CHANGED,
		.handler = &onTimeChanged
	};

	eventSubscribe(&onTimeChangedSubscription);
}

static void onTimeChanged(const struct Event *const event)
{
	timeChanged = (const struct TimeChanged *) event->args;
	timeChangedCalls++;
	timeChangedSequence = stubCallSequence++;
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

static void mockOnDateChanged(void)
{
	static const struct EventSubscription onDateChangedSubscription =
	{
		.type = DATE_CHANGED,
		.handler = &onDateChanged
	};

	eventSubscribe(&onDateChangedSubscription);
}

static void onDateChanged(const struct Event *const event)
{
	dateChanged = (const struct DateChanged *) event->args;
	dateChangedCalls++;
	dateChangedSequence = stubCallSequence++;
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
