#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/PowerManagement.h"
#include "Platform/Event.h"
#include "Platform/Clock.h"

#include "ClockFixture.h"
#include "../../NonDeterminism.h"

static uint8_t stubCallSequence;

const struct DateChanged *dateChanged;
uint8_t dateChangedCalls;
uint8_t dateChangedSequence;

const struct TimeChanged *timeChanged;
uint8_t timeChangedCalls;
uint8_t timeChangedSequence;

void clockFixtureSetUp(void)
{
	stubCallSequence = 1;

	dateChanged = (const struct DateChanged *) 0;
	dateChangedCalls = 0;
	dateChangedSequence = 0;

	timeChanged = (const struct TimeChanged *) 0;
	timeChangedCalls = 0;
	timeChangedSequence = 0;
}

void clockFixtureTearDown(void)
{
}

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
	const struct DateAndTimeGet *expected,
	const struct DateAndTimeGet *actual)
{
	assertEqualDateTimeExceptMinute(expected, actual);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->time.minute, actual->time.minute, "MM");
}

void assertEqualDateTimeExceptMinute(
	const struct DateAndTimeGet *expected,
	const struct DateAndTimeGet *actual)
{
	assertEqualDateTimeExceptHourAndMinute(expected, actual);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->time.hour, actual->time.hour, "HH");
}

void assertEqualDateTimeExceptHourAndMinute(
	const struct DateAndTimeGet *expected,
	const struct DateAndTimeGet *actual)
{
	assertEqualDateTimeExceptDayAndHourAndMinute(expected, actual);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->date.day, actual->date.day, "DD");

	TEST_ASSERT_EQUAL_UINT16_MESSAGE(
		expected->date.dayOfYear, actual->date.dayOfYear, "DoY");
}

void assertEqualDateTimeExceptDayAndHourAndMinute(
	const struct DateAndTimeGet *expected,
	const struct DateAndTimeGet *actual)
{
	assertEqualDateTimeExceptMonthAndDayAndHourAndMinute(expected, actual);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->date.month, actual->date.month, "M");
}

void assertEqualDateTimeExceptMonthAndDayAndHourAndMinute(
	const struct DateAndTimeGet *expected,
	const struct DateAndTimeGet *actual)
{
	assertEqualDateTimeExceptYearAndMonthAndDayAndHourAndMinute(expected, actual);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->date.year, actual->date.year, "Y");
}

void assertEqualDateTimeExceptYearAndMonthAndDayAndHourAndMinute(
	const struct DateAndTimeGet *expected,
	const struct DateAndTimeGet *actual)
{
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->time.second, actual->time.second, "SS");
}

void assertEqualDate(const struct Date *expected, const struct Date *actual)
{
	TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, sizeof(struct Date));
}

void assertEqualTime(
	const struct Time *expected,
	const struct Time *actual)
{
	TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, sizeof(struct Time));
}

void mockOnDateChanged(void)
{
	static const struct EventSubscription onDateChangedSubscription =
	{
		.type = DATE_CHANGED,
		.handler = &onDateChanged
	};

	eventSubscribe(&onDateChangedSubscription);
}

void onDateChanged(const struct Event *event)
{
	dateChanged = (const struct DateChanged *) event->args;
	dateChangedCalls++;
	dateChangedSequence = stubCallSequence++;
}

void mockOnTimeChanged(void)
{
	static const struct EventSubscription onTimeChangedSubscription =
	{
		.type = TIME_CHANGED,
		.handler = &onTimeChanged
	};

	eventSubscribe(&onTimeChangedSubscription);
}

void onTimeChanged(const struct Event *event)
{
	timeChanged = (const struct TimeChanged *) event->args;
	timeChangedCalls++;
	timeChangedSequence = stubCallSequence++;
}
