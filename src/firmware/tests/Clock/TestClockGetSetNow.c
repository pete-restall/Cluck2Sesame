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
	stubAnyDateTimeWithMinute(anyByteLessThan(59));

	struct DateAndTimeGet before;
	clockGetNowGmt(&before);

	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(TMR0L, now.time.second, "SS");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(before.time.minute + 1, now.time.minute, "MM");
	assertEqualDateTimeExceptMinute(&before, &now);
}

void test_clockGetNowGmt_calledWhenClockOf59MinutesTicks_expectNewHourAndZeroMinutes(void)
{
	TMR0L = anyByte();
	stubAnyDateTimeWithHourAndMinute(anyByteLessThan(23), 59);

	struct DateAndTimeGet before;
	clockGetNowGmt(&before);

	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.time.minute, "MM");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(before.time.hour + 1, now.time.hour, "HH");
	assertEqualDateTimeExceptHourAndMinute(&before, &now);
}

void test_clockGetNowGmt_calledWhenClockOf23HoursAnd59MinutesTicks_expectNewDayAndZeroHoursAndMinutes(void)
{
	TMR0L = anyByte();
	stubAnyDateTimeWithDayAndHourAndMinute(
		1 + anyByteLessThan(28),
		23,
		59);

	struct DateAndTimeGet before;
	clockGetNowGmt(&before);

	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.time.minute, "MM");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.time.hour, "HH");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(before.date.day + 1, now.date.day, "D");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(before.date.dayOfYear + 1, now.date.dayOfYear, "DoY");
	assertEqualDateTimeExceptDayAndHourAndMinute(&before, &now);
}

void test_clockGetNowGmt_calledWhenClockOfLastDayOfNonLeapYearMonthTicks_expectNewMonthAndFirstDayAndZeroHoursAndMinutes(void)
{
	static const uint8_t daysInMonth[] =
	{
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30
	};

	for (uint8_t month = 0; month < 11; month++)
	{
		struct DateAndTimeGet before =
		{
			.date =
			{
				.year = anyByteLessThan(99),
				.month = 1 + month,
				.day = daysInMonth[month]
			},
			.time = { .hour = 23, .minute = 59, .second = anyByteLessThan(60) }
		};

		if (before.date.year % 4 == 0)
			before.date.year++;

		clockSetNowGmt(&before);
		clockGetNowGmt(&before);

		tick();
		struct DateAndTimeGet now;
		clockGetNowGmt(&now);

		TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.time.minute, "MM");
		TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.time.hour, "HH");
		TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, now.date.day, "D");
		TEST_ASSERT_EQUAL_UINT8_MESSAGE(before.date.dayOfYear + 1, now.date.dayOfYear, "DoY");
		TEST_ASSERT_EQUAL_UINT8_MESSAGE(before.date.month + 1, now.date.month, "M");
		assertEqualDateTimeExceptMonthAndDayAndHourAndMinute(&before, &now);
	}
}

void test_clockGetNowGmt_calledWhenClockOfLastDayOfNonLeapYearDecemberTicks_expectNewYearAndAndFirstMonthAndDayAndZeroHoursAndMinutes(void)
{
	struct DateAndTimeGet before =
	{
		.date =
		{
			.year = anyByteLessThan(99),
			.month = 12,
			.day = 31
		},
		.time = { .hour = 23, .minute = 59, .second = anyByteLessThan(60) }
	};

	if (before.date.year % 4 == 0)
		before.date.year++;

	clockSetNowGmt(&before);
	clockGetNowGmt(&before);

	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.time.minute, "MM");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.time.hour, "HH");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, now.date.day, "D");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.date.dayOfYear, "DoY");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, now.date.month, "M");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(before.date.year + 1, now.date.year, "Y");
	assertEqualDateTimeExceptYearAndMonthAndDayAndHourAndMinute(&before, &now);
}

// void test_clockGetNowGmt_calledWhenClockOfLastDayOfLeapYearDecemberTicks_expectNewYearAndAndFirstMonthAndDayAndZeroHoursAndMinutes(void)

// void test_clockGetNowGmt_calledWhenClockOfLastDayOfCenturyDecemberTicks_expectZeroYearAndAndFirstMonthAndDayAndZeroHoursAndMinutes(void)

// TODO: CLOCK GET WHEN TMR0IF IS SET - MAKE SURE IT'S CLEARED AND MINUTES ARE UPDATED


// TODO: CLOCK SET WHEN TMR0IF IS SET - MAKE SURE IT'S CLEARED AND MINUTES NOT UPDATED
