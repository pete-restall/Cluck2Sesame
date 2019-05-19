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

void setUp(void)
{
	eventInitialise();
	clockInitialise();
	T0CON0bits.T0EN = 0;
	T0CON1bits.T0CKPS = 0;
}

void tearDown(void)
{
}

void test_tick_onWokenFromSleepWhenTimerInterruptFlagIsClear_expectTimeIsNotIncremented(void)
{
	TMR0H = 1;

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
	while (eventDispatchNext())
		;;
}

static void publishWokenFromSleep(void)
{
	static const struct WokenFromSleep emptyArgs = { };
	eventPublish(WOKEN_FROM_SLEEP, &emptyArgs);
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
