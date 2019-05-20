#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Clock.h"

#include "ClockFixture.h"
#include "../NonDeterminism.h"

void stubAnyDateTimeWithHourAndMinute(uint8_t hour, uint8_t minute)
{
	stubAnyDateTimeWithDayAndHourAndMinute(
		1 + anyByteLessThan(28),
		hour,
		minute);
}

void stubAnyDateTimeWithDayAndHourAndMinute(
	uint8_t day,
	uint8_t hour,
	uint8_t minute)
{
	stubAnyDateTime();

	struct DateAndTimeGet now;
	clockGetNowGmt(&now);
	now.date.day = day;
	now.time.hour = hour;
	now.time.minute = minute;
	clockSetNowGmt((const struct DateAndTimeSet *) &now);
}

void stubAnyDateTime(void)
{
	struct DateAndTimeSet now =
	{
		.date =
		{
			.year = anyByteLessThan(100),
			.month = 1 + anyByteLessThan(12),
			.day = 1 + anyByteLessThan(28)
		},

		.time =
		{
			.hour = anyByteLessThan(24),
			.minute = anyByteLessThan(60),
			.second = anyByteLessThan(60)
		}
	};

	clockSetNowGmt(&now);
}

void assertEqualDateTime(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual)
{
	assertEqualDateTimeExceptMinute(expected, actual);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->time.minute, actual->time.minute, "MM");
}

void assertEqualDateTimeExceptMinute(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual)
{
	assertEqualDateTimeExceptHourAndMinute(expected, actual);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->time.hour, actual->time.hour, "HH");
}

void assertEqualDateTimeExceptHourAndMinute(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual)
{
	assertEqualDateTimeExceptDayAndHourAndMinute(expected, actual);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->date.day, actual->date.day, "DD");

	TEST_ASSERT_EQUAL_UINT16_MESSAGE(
		expected->date.dayOfYear, actual->date.dayOfYear, "DoY");
}

void assertEqualDateTimeExceptDayAndHourAndMinute(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual)
{
	assertEqualDateTimeExceptMonthAndDayAndHourAndMinute(expected, actual);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->date.month, actual->date.month, "M");
}

void assertEqualDateTimeExceptMonthAndDayAndHourAndMinute(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual)
{
	assertEqualDateTimeExceptYearAndMonthAndDayAndHourAndMinute(expected, actual);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->date.year, actual->date.year, "Y");
}

void assertEqualDateTimeExceptYearAndMonthAndDayAndHourAndMinute(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual)
{
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->time.second, actual->time.second, "SS");
}
