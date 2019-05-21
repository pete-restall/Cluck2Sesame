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
	clockFixtureSetUp();
	clockGetSetNowFixtureSetUp();
}

void tearDown(void)
{
	clockGetSetNowFixtureTearDown();
	clockFixtureTearDown();
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

void test_clockSetNowGmt_calledWhenTimerInterruptFlagIsSet_expectTimerInterruptFlagIsCleared(void)
{
	struct DateAndTimeGet now;
	stubAnyDateTime();
	clockGetNowGmt(&now);

	PIR0 = anyByteWithMaskSet(_PIR0_TMR0IF_MASK);
	uint8_t expectedPir0 = PIR0 & ~_PIR0_TMR0IF_MASK;

	clockSetNowGmt((const struct DateAndTimeSet *) &now);
	TEST_ASSERT_EQUAL_UINT8(expectedPir0, PIR0);
}

void test_clockSetNowGmt_calledWhenTmr0hIsNotOneMinute_expectTmr0hIsResetToOneMinute(void)
{
	struct DateAndTimeGet now;
	stubAnyDateTime();
	clockGetNowGmt(&now);

	TMR0H = anyByteExcept(59);

	clockSetNowGmt((const struct DateAndTimeSet *) &now);
	TEST_ASSERT_EQUAL_UINT8(59, TMR0H);
}

// TODO: TEST SETNOW TO MAKE SURE DATE_CHANGED HAS LEAP-YEAR FLAG SET / CLEAR
// TODO: TEST SETNOW TO MAKE SURE DATE_CHANGED HAS LEAP-YEAR FLAG SET / CLEAR
// TODO: TEST SETNOW TO MAKE SURE DATE_CHANGED HAS DST FLAG SET / CLEAR
// TODO: TEST GETNOW TO MAKE SURE DATE_CHANGED HAS DST FLAG SET / CLEAR
// TODO: TESTS TO MAKE SURE THE TIME_CHANGED EVENT IS FIRED AFTER THE DATE_CHANGED EVENT...
