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

void onBeforeTest(void)
{
	clockFixtureSetUp();
	clockGetSetNowFixtureSetUp();
}

void onAfterTest(void)
{
	clockGetSetNowFixtureTearDown();
	clockFixtureTearDown();
}

void test_clockGetNowGmt_called_expectSecondsIsTmr0l(void)
{
	stubAnyDateTime();
	TMR0L = anyByte();

	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_EQUAL_UINT8(TMR0L, now.time.second);
}

void test_clockGetNowGmt_calledWhenClockLessThan59MinutesTicks_expectNewMinute(void)
{
	TMR0L = anyByte();
	stubAnyDateTimeWithHourAndMinute(anyByteLessThan(23), anyByteLessThan(59));

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
				.year = anyNonLeapYearLessThan(99),
				.month = 1 + month,
				.day = daysInMonth[month]
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
		TEST_ASSERT_EQUAL_UINT8_MESSAGE(before.date.dayOfYear + 1, now.date.dayOfYear, "DoY");
		TEST_ASSERT_EQUAL_UINT8_MESSAGE(before.date.month + 1, now.date.month, "M");
		assertEqualDateTimeExceptMonthAndDayAndHourAndMinute(&before, &now);
	}
}

void test_clockGetNowGmt_calledWhenClockOf28FebruaryInLeapYearTicks_expectLastDayOfFebruaryAndZeroHoursAndMinutes(void)
{
	struct DateAndTimeGet before =
	{
		.date =
		{
			.year = anyLeapYear(),
			.month = 2,
			.day = 28
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
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(29, now.date.day, "D");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(before.date.dayOfYear + 1, now.date.dayOfYear, "DoY");
	assertEqualDateTimeExceptDayAndHourAndMinute(&before, &now);
}

void test_clockGetNowGmt_calledWhenClockOfLastDayOfFebruaryInLeapYearMonthTicks_expectNewMonthAndFirstDayAndZeroHoursAndMinutes(void)
{
	struct DateAndTimeGet before =
	{
		.date =
		{
			.year = anyLeapYear(),
			.month = 2,
			.day = 29
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
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(before.date.dayOfYear + 1, now.date.dayOfYear, "DoY");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(before.date.month + 1, now.date.month, "M");
	assertEqualDateTimeExceptMonthAndDayAndHourAndMinute(&before, &now);
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
