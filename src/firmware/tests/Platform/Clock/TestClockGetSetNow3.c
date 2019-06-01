#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/PowerManagement.h"
#include "Platform/Clock.h"

#include "ClockFixture.h"
#include "ClockGetSetNowFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Clock/ClockInitialise.c")
TEST_FILE("Platform/Clock/ClockGetSetNow.c")

void onBeforeTest(void)
{
	clockFixtureSetUp();
	clockGetSetNowFixtureSetUp();
	dispatchAllEvents();
}

void onAfterTest(void)
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

void test_clockSetNowGmt_calledWhenLeapYear_expectIsLeapYearFlagIsSet(void)
{
	struct DateAndTimeSet now =
	{
		.date =
		{
			.year = anyLeapYear(),
			.month = 1 + anyByteLessThan(12),
			.day = 1 + anyByteLessThan(29)
		}
	};

	mockOnDateChanged();
	clockSetNowGmt(&now);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, dateChangedCalls, "Calls");
	TEST_ASSERT_NOT_NULL_MESSAGE(dateChanged, "Args");
	TEST_ASSERT_TRUE_MESSAGE(dateChanged->today->flags.isLeapYear, "Flag");
}

void test_clockSetNowGmt_calledWhenNotLeapYear_expectIsLeapYearFlagIsClear(void)
{
	struct DateAndTimeSet now =
	{
		.date =
		{
			.year = anyNonLeapYear(),
			.month = 1 + anyByteLessThan(12),
			.day = 1 + anyByteLessThan(28)
		}
	};

	mockOnDateChanged();
	clockSetNowGmt(&now);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, dateChangedCalls, "Calls");
	TEST_ASSERT_NOT_NULL_MESSAGE(dateChanged, "Args");
	TEST_ASSERT_FALSE_MESSAGE(dateChanged->today->flags.isLeapYear, "Flag");
}

void test_clockSetNowGmt_called_expectDateChangedIsPublishedWithCorrectDate(void)
{
	struct DateAndTimeSet now =
	{
		.date =
		{
			.year = anyByteLessThan(100),
			.month = 1 + anyByteLessThan(12),
			.day = 1 + anyByteLessThan(28)
		}
	};

	mockOnDateChanged();
	clockSetNowGmt(&now);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, dateChangedCalls, "Calls");
	TEST_ASSERT_NOT_NULL_MESSAGE(dateChanged, "Args");
	assertEqualDate(&now.date, (const struct Date *) dateChanged->today);
}

void test_clockSetNowGmt_called_expectTimeChangedIsPublishedAfterDateChanged(void)
{
	struct DateAndTimeSet now =
	{
		.date =
		{
			.year = anyNonLeapYear(),
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

	mockOnDateChanged();
	mockOnTimeChanged();
	clockSetNowGmt(&now);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, dateChangedCalls, "Calls (D)");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, timeChangedCalls, "Calls (T)");
	TEST_ASSERT_TRUE_MESSAGE(dateChangedSequence < timeChangedSequence, "Sequence");
}

void test_clockSetNowGmt_called_expectTimeChangedIsPublishedWithCorrectTime(void)
{
	struct DateAndTimeSet now =
	{
		.time =
		{
			.hour = anyByteLessThan(24),
			.minute = anyByteLessThan(60),
			.second = anyByteLessThan(60)
		}
	};

	mockOnDateChanged();
	mockOnTimeChanged();
	clockSetNowGmt(&now);
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(timeChanged);
	assertEqualTime(&now.time, timeChanged->now);
}

// TODO: TEST SETNOW TO MAKE SURE DATE_CHANGED HAS DST FLAG SET / CLEAR
// TODO: TEST GETNOW TO MAKE SURE DATE_CHANGED HAS DST FLAG SET / CLEAR
