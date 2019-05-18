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
}

void tearDown(void)
{
}

void test_clockGetNowGmt_calledWhenClockTicks_expectSecondsIncrementedByTmr0h(void)
{
	TMR0H = anyByteLessThan(60);
	TMR0L = 0;
	stubAnyDateTimeWithSeconds(0);

	struct DateAndTimeGet before;
	clockGetNowGmt(&before);

	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_EQUAL_UINT8(TMR0H, now.time.second);
	assertEqualDateTimeExceptSecond(&before, &now);
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

void test_clockGetNowGmt_calledWhenClockTicks0To60_expectNewMinute(void)
{
	TMR0H = 60;
	TMR0L = 0;
	stubAnyDateTimeWithMinutesAndSeconds(anyByteLessThan(59), 0);

	struct DateAndTimeGet before;
	clockGetNowGmt(&before);

	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_EQUAL_UINT8(0, now.time.second);
	TEST_ASSERT_EQUAL_UINT8(before.time.minute + 1, now.time.minute);
	assertEqualDateTimeExceptMinuteAndSecond(&before, &now);
}

void test_clockGetNowGmt_calledWhenClockTicks0To61_expectNewMinuteWithResidualSeconds(void)
{
	TMR0H = 61;
	TMR0L = 0;
	stubAnyDateTimeWithMinutesAndSeconds(anyByteLessThan(59), 0);

	struct DateAndTimeGet before;
	clockGetNowGmt(&before);

	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_EQUAL_UINT8(1, now.time.second);
	TEST_ASSERT_EQUAL_UINT8(before.time.minute + 1, now.time.minute);
	assertEqualDateTimeExceptMinuteAndSecond(&before, &now);
}

void test_clockGetNowGmt_calledWhenClockTicks59To62_expectNewMinuteWithResidualSeconds(void)
{
	TMR0H = 3;
	TMR0L = 0;
	stubAnyDateTimeWithMinutesAndSeconds(anyByteLessThan(59), 59);

	struct DateAndTimeGet before;
	clockGetNowGmt(&before);

	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_EQUAL_UINT8(2, now.time.second);
	TEST_ASSERT_EQUAL_UINT8(before.time.minute + 1, now.time.minute);
	assertEqualDateTimeExceptMinuteAndSecond(&before, &now);
}

void test_clockGetNowGmt_calledWhenClockTicks0To255_expectNewMinuteWithResidualSeconds(void)
{
	TMR0H = 255;
	TMR0L = 0;
	stubAnyDateTimeWithMinutesAndSeconds(anyByteLessThan(56), 0);

	struct DateAndTimeGet before;
	clockGetNowGmt(&before);

	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_EQUAL_UINT8(15, now.time.second);
	TEST_ASSERT_EQUAL_UINT8(before.time.minute + 4, now.time.minute);
	assertEqualDateTimeExceptMinuteAndSecond(&before, &now);
}

void test_clockGetNowGmt_calledWhenClockTicks0To256_expectNewMinuteWithResidualSeconds(void)
{
	TMR0H = 0;
	TMR0L = 0;
	stubAnyDateTimeWithMinutesAndSeconds(anyByteLessThan(56), 0);

	struct DateAndTimeGet before;
	clockGetNowGmt(&before);

	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_EQUAL_UINT8(16, now.time.second);
	TEST_ASSERT_EQUAL_UINT8(before.time.minute + 4, now.time.minute);
	assertEqualDateTimeExceptMinuteAndSecond(&before, &now);
}

void test_clockGetNowGmt_calledWhenClockTicks7To263_expectNewMinuteWithResidualSeconds(void)
{
	TMR0H = 0;
	TMR0L = 0;
	stubAnyDateTimeWithMinutesAndSeconds(anyByteLessThan(56), 7);

	struct DateAndTimeGet before;
	clockGetNowGmt(&before);

	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_EQUAL_UINT8(23, now.time.second);
	TEST_ASSERT_EQUAL_UINT8(before.time.minute + 4, now.time.minute);
	assertEqualDateTimeExceptMinuteAndSecond(&before, &now);
}
