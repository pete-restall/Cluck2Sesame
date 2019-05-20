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

static void clockGetNowGmt_called_expectDayOfYearIsValidForEachMonth(
	struct DateAndTimeSet *const monthPrototype,
	const uint16_t *expectedDayOfYear);

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

void test_clockGetNowGmt_calledWhenClockOfLastDayOfCenturyTicks_expectZeroYearAndAndFirstMonthAndDayAndZeroHoursAndMinutes(void)
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

void test_clockGetNowGmt_calledWhenClockOfLastDayOfLeapYearDecemberTicks_expectIsLeapYearFlagIsCleared(void)
{
	struct DateAndTimeSet before =
	{
		.date =
		{
			.year = anyLeapYear(),
			.month = 12,
			.day = 31
		},
		.time = { .hour = 23, .minute = 59, .second = anyByteLessThan(60) }
	};

	clockSetNowGmt(&before);
	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_FALSE(now.date.flags.isLeapYear);
}

void test_clockGetNowGmt_calledWhenClockOfLastDayOfNonLeapYearDecemberTicksIntoLeapYear_expectIsLeapYearFlagIsSet(void)
{
	struct DateAndTimeSet before =
	{
		.date =
		{
			.year = anyLeapYear() + 3,
			.month = 12,
			.day = 31
		},
		.time = { .hour = 23, .minute = 59, .second = anyByteLessThan(60) }
	};

	clockSetNowGmt(&before);
	tick();
	struct DateAndTimeGet now;
	clockGetNowGmt(&now);

	TEST_ASSERT_TRUE(now.date.flags.isLeapYear);
}

void test_clockGetNowGmt_calledWhenLeapYear_expectIsLeapYearFlagIsSet(void)
{
	struct DateAndTimeSet before =
	{
		.date =
		{
			.year = 0,
			.month = 12,
			.day = 31
		},
		.time = { .hour = 23, .minute = 59, .second = anyByteLessThan(60) }
	};

	for (uint8_t leapYear = 0; leapYear < 100; leapYear += 4)
	{
		before.date.year = leapYear;
		clockSetNowGmt(&before);

		struct DateAndTimeGet now;
		clockGetNowGmt(&now);
		TEST_ASSERT_TRUE(now.date.flags.isLeapYear);
	}
}

void test_clockGetNowGmt_calledWhen1JanuaryInAllYears_expectDayOfYearIs0(void)
{
	struct DateAndTimeSet firstDayOfYear =
	{
		.date =
		{
			.year = 0,
			.month = 1,
			.day = 1
		}
	};

	for (uint8_t year = 0; year < 100; year++)
	{
		firstDayOfYear.date.year = year;
		clockSetNowGmt(&firstDayOfYear);

		struct DateAndTimeGet now;
		clockGetNowGmt(&now);
		TEST_ASSERT_EQUAL_UINT16(0, now.date.dayOfYear);
	}
}

void test_clockGetNowGmt_calledWhen31DecemberInLeapYear_expectDayOfYearIs365(void)
{
	struct DateAndTimeSet lastDayOfYear =
	{
		.date =
		{
			.year = 0,
			.month = 12,
			.day = 31
		}
	};

	for (uint8_t leapYear = 0; leapYear < 100; leapYear += 4)
	{
		lastDayOfYear.date.year = leapYear;
		clockSetNowGmt(&lastDayOfYear);

		struct DateAndTimeGet now;
		clockGetNowGmt(&now);
		TEST_ASSERT_EQUAL_UINT16(365, now.date.dayOfYear);
	}
}

void test_clockGetNowGmt_calledWhen31DecemberInNonLeapYear_expectDayOfYearIs364(void)
{
	struct DateAndTimeSet lastDayOfYear =
	{
		.date =
		{
			.year = 1,
			.month = 12,
			.day = 31
		}
	};

	for (uint8_t nonLeapYear = 1; nonLeapYear < 100; nonLeapYear++)
	{
		if ((nonLeapYear & 3) == 0)
			continue;

		lastDayOfYear.date.year = nonLeapYear;
		clockSetNowGmt(&lastDayOfYear);

		struct DateAndTimeGet now;
		clockGetNowGmt(&now);
		TEST_ASSERT_EQUAL_UINT16(364, now.date.dayOfYear);
	}
}

void test_clockGetNowGmt_calledWhenNotLeapYear_expectIsLeapYearFlagIsClear(void)
{
	struct DateAndTimeSet before =
	{
		.date =
		{
			.year = 1,
			.month = 12,
			.day = 31
		},
		.time = { .hour = 23, .minute = 59, .second = anyByteLessThan(60) }
	};

	for (uint8_t nonLeapYear = 1; nonLeapYear < 100; nonLeapYear++)
	{
		if ((nonLeapYear & 3) == 0)
			continue;

		before.date.year = nonLeapYear;
		clockSetNowGmt(&before);

		struct DateAndTimeGet now;
		clockGetNowGmt(&now);
		TEST_ASSERT_FALSE(now.date.flags.isLeapYear);
	}
}

void test_clockGetNowGmt_calledWhenNotLeapYear_expectDayOfYearIsValidForEachMonthStart(void)
{
	static const uint16_t expectedDayOfYear[] =
	{
		0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
	};

	struct DateAndTimeSet monthStart =
	{
		.date =
		{
			.year = anyNonLeapYear(),
			.month = 1,
			.day = 1
		}
	};

	clockGetNowGmt_called_expectDayOfYearIsValidForEachMonth(
		&monthStart,
		&expectedDayOfYear);
}

static void clockGetNowGmt_called_expectDayOfYearIsValidForEachMonth(
	struct DateAndTimeSet *const monthPrototype,
	const uint16_t *expectedDayOfYear)
{
	static const int8_t daysInMonth[] =
	{
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};

	int8_t prototypeDay = (int8_t) monthPrototype->date.day;
	for (uint8_t month = 0; month < 12; month++)
	{
		monthPrototype->date.month = 1 + month;

		if (prototypeDay < 1)
		{
			int8_t leapYearFebruaryAdjustment =
				month == 1 && (monthPrototype->date.year & 3) == 0
					? 1
					: 0;

			monthPrototype->date.day = (uint8_t)
				prototypeDay +
				daysInMonth[month] +
				leapYearFebruaryAdjustment;
		}

		clockSetNowGmt(monthPrototype);

		struct DateAndTimeGet now;
		clockGetNowGmt(&now);
		TEST_ASSERT_EQUAL_UINT16(*(expectedDayOfYear++), now.date.dayOfYear);
	}
}

void test_clockGetNowGmt_calledWhenLeapYear_expectDayOfYearIsValidForEachMonthStart(void)
{
	static const uint16_t expectedDayOfYear[] =
	{
		0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335
	};

	struct DateAndTimeSet monthStart =
	{
		.date =
		{
			.year = anyLeapYear(),
			.month = 1,
			.day = 1
		}
	};

	clockGetNowGmt_called_expectDayOfYearIsValidForEachMonth(
		&monthStart,
		&expectedDayOfYear);
}

void test_clockGetNowGmt_calledWhenNotLeapYear_expectDayOfYearIsValidForEachMonthEnd(void)
{
	static const uint16_t expectedDayOfYear[] =
	{
		30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364
	};

	struct DateAndTimeSet monthEnd =
	{
		.date =
		{
			.year = anyNonLeapYear(),
			.month = 1,
			.day = 0
		}
	};

	clockGetNowGmt_called_expectDayOfYearIsValidForEachMonth(
		&monthEnd,
		&expectedDayOfYear);
}

void test_clockGetNowGmt_calledWhenLeapYear_expectDayOfYearIsValidForEachMonthEnd(void)
{
	static const uint16_t expectedDayOfYear[] =
	{
		30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
	};

	struct DateAndTimeSet monthEnd =
	{
		.date =
		{
			.year = anyLeapYear(),
			.month = 1,
			.day = 0
		}
	};

	clockGetNowGmt_called_expectDayOfYearIsValidForEachMonth(
		&monthEnd,
		&expectedDayOfYear);
}
