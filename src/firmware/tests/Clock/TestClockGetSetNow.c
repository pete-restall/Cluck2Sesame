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
TEST_FILE("Clock/ClockDateFromDiscrete.c")
TEST_FILE("Clock/ClockTimeFromDiscrete.c")

static void tick(void);
static void publishWokenFromSleep(void);

void setUp(void)
{
	eventInitialise();
	clockInitialise();
	T0CON0bits.T0EN = 0;
	T0CON1bits.T0CKPS = 0;

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(59, TMR0H, "60-second timebase required !");
}

void tearDown(void)
{
}

void test_clockGetNowGmt_called_expectSecondsIsTmr0l(void)
{
	stubAnyDateTime();
	TMR0L = anyByte();

	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_EQUAL_UINT8(TMR0L, now.time.second);
}

static void tick(void)
{
	PIR0bits.TMR0IF = 1;
	publishWokenFromSleep();
	while (eventDispatchNext())
		;;
}

static void publishWokenFromSleep(void)
{
	static const struct WokenFromSleep emptyArgs = { };
	eventPublish(WOKEN_FROM_SLEEP, &emptyArgs);
}

void test_clockGetNowGmt_calledWhenClockLessThan59MinutesTicks_expectNewMinute(void)
{
	TMR0L = anyByte();
	stubAnyDateTimeWithMinutes(anyByteLessThan(59));

	struct DateAndTimeGet before;
	clockGetNowGmt(&before);

	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(TMR0L, now.time.second, "SS");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(before.time.minute + 1, now.time.minute, "MM");
	assertEqualDateTimeExceptMinute(&before, &now);
}

// TODO: CLOCK SET WHEN TMR0IF IS SET - MAKE SURE IT'S CLEARED AND MINUTES NOT UPDATED
// TODO: CLOCK GET WHEN TMR0IF IS SET - MAKE SURE IT'S CLEARED AND MINUTES ARE UPDATED
