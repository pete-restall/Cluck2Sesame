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
TEST_FILE("Clock/ClockDateFromDiscrete.c")
TEST_FILE("Clock/ClockTimeFromDiscrete.c")

void setUp(void)
{
	clockGetSetNowFixtureSetUp();
}

void tearDown(void)
{
	clockGetSetNowFixtureTearDown();
}

void test_clockGetNowGmt_calledWhenClockOfLastDayOfNonLeapYearDecemberTicks_expectNewYearAndAndFirstMonthAndDayAndZeroHoursAndMinutes(void)
{
	struct DateAndTimeGet before =
	{
		.date =
		{
			.year = anyNonLeapYearLessThan(99),
			.month = 12,
			.day = 31
		},
		.time = { .hour = 23, .minute = 59, .second = anyByteLessThan(60) }
	};

	clockSetNowGmt((const struct DateAndTimeSet *) &before);
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

void test_clockGetNowGmt_calledWhenClockOfLastDayOfLeapYearDecemberTicks_expectNewYearAndAndFirstMonthAndDayAndZeroHoursAndMinutes(void)
{
	struct DateAndTimeGet before =
	{
		.date =
		{
			.year = anyLeapYear(),
			.month = 12,
			.day = 31
		},
		.time = { .hour = 23, .minute = 59, .second = anyByteLessThan(60) }
	};

	clockSetNowGmt((const struct DateAndTimeSet *) &before);
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

void test_clockGetNowGmt_calledWhenClockOfLastDayOfCenturyDecemberTicks_expectZeroYearAndAndFirstMonthAndDayAndZeroHoursAndMinutes(void)
{
	struct DateAndTimeGet before =
	{
		.date =
		{
			.year = 99,
			.month = 12,
			.day = 31
		},
		.time = { .hour = 23, .minute = 59, .second = anyByteLessThan(60) }
	};

	clockSetNowGmt((const struct DateAndTimeSet *) &before);
	clockGetNowGmt(&before);

	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.time.minute, "MM");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.time.hour, "HH");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, now.date.day, "D");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.date.dayOfYear, "DoY");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, now.date.month, "M");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.date.year, "Y");
	assertEqualDateTimeExceptYearAndMonthAndDayAndHourAndMinute(&before, &now);
}

// TODO: WHEN TICK ROLLS OVER FROM LEAP-YEAR TO NON-LEAP-YEAR, EXPECT LEAP-YEAR FLAG IS CLEARED

// TODO: WHEN TICK ROLLS OVER FROM NON-LEAP-YEAR TO LEAP-YEAR, EXPECT LEAP-YEAR FLAG IS SET

// TODO: GET CLOCK WHEN LEAP-YEAR, EXPECT LEAP-YEAR FLAG IS SET

// TODO: GET CLOCK WHEN NOT LEAP-YEAR, EXPECT LEAP-YEAR FLAG IS CLEAR

// TODO: CLOCK GET WHEN TMR0IF IS SET - MAKE SURE IT'S CLEARED AND MINUTES ARE UPDATED

// TODO: CLOCK SET WHEN TMR0IF IS SET - MAKE SURE IT'S CLEARED AND MINUTES NOT UPDATED
