#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Clock.h"

#include "ClockFixture.h"
#include "../NonDeterminism.h"

void stubAnyDateTimeWithSeconds(uint8_t seconds)
{
	stubAnyDateTimeWithMinutesAndSeconds(
		anyByteLessThan(60),
		seconds);
}

void stubAnyDateTimeWithMinutesAndSeconds(uint8_t minutes, uint8_t seconds)
{
	stubAnyDateTime();

	struct DateAndTimeGet now;
	clockGetNowGmt(&now);
	now.time.minute = minutes;
	now.time.second = seconds;
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
	assertEqualDateTimeExceptSecond(expected, actual);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->time.second, actual->time.second, "SS");
}

void assertEqualDateTimeExceptSecond(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual)
{
	assertEqualDateTimeExceptMinuteAndSecond(expected, actual);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->time.minute, actual->time.minute, "MM");
}

void assertEqualDateTimeExceptMinuteAndSecond(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual)
{
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->date.year, actual->date.year, "Y");

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->date.month, actual->date.month, "M");

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->date.day, actual->date.day, "D");

	TEST_ASSERT_EQUAL_UINT16_MESSAGE(
		expected->date.dayOfYear, actual->date.dayOfYear, "DoY");

	TEST_ASSERT_EQUAL_HEX8_MESSAGE(
		expected->date.flags.all, actual->date.flags.all, "F");

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->time.hour, actual->time.hour, "HH");
}
