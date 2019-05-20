#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Event.h"
#include "PowerManagement.h"
#include "Clock.h"

#include "ClockFixture.h"
#include "ClockGetSetNowFixture.h"

#include "../NonDeterminism.h"

TEST_FILE("Clock/ClockInitialise.c")
TEST_FILE("Clock/ClockGetSetNow.c")

void setUp(void)
{
	clockGetSetNowFixtureSetUp();
}

void tearDown(void)
{
	clockGetSetNowFixtureTearDown();
}

void test_clockGetNowGmt_calledWhenTimerInterruptFlagIsSet_expectTimeIsSameAsWouldBeAfterTick(void)
{
	struct DateAndTimeGet before, after;
	stubAnyDateTime();
	clockGetNowGmt(&before);
	tick();
	clockGetNowGmt(&after);

	clockSetNowGmt((const struct DateAndTimeSet *) &before);

	PIR0bits.TMR0IF = 1;
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);
	assertEqualDateTime(&after, &now);
}

void test_clockGetNowGmt_calledWhenTimerInterruptFlagIsSet_expectTimerInterruptFlagIsCleared(void)
{
	struct DateAndTimeGet before, after;
	stubAnyDateTime();
	clockGetNowGmt(&before);
	tick();
	clockGetNowGmt(&after);

	clockSetNowGmt((const struct DateAndTimeSet *) &before);

	PIR0 = anyByteWithMaskSet(_PIR0_TMR0IF_MASK);
	uint8_t expectedPir0 = PIR0 & ~_PIR0_TMR0IF_MASK;

	struct DateAndTimeGet now;
	clockGetNowGmt(&now);
	TEST_ASSERT_EQUAL_UINT8(expectedPir0, PIR0);
}

// TODO: TESTS TO MAKE SURE THE EVENTS ARE FIRED...
