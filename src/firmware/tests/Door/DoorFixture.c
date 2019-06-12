#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/NvmSettings.h"
#include "ApplicationNvmSettings.h"
#include "Door.h"

#include "DoorFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"
#include "../NvmSettingsFixture.h"

uint8_t farSchedulerAddCalls;
const struct FarSchedule *farSchedulerAddArgs[8];

void doorFixtureInitialise(void)
{
	farSchedulerAddCalls = 0;
	eventInitialise();
	doorInitialise();
}

void doorFixtureShutdown(void)
{
}

void stubNvmSettingsForTimeDrivenMode(void)
{
	union ApplicationNvmSettings settings =
	{
		.door =
		{
			.mode =
			{
				.isTimeDriven = 1,
				.isManuallyOverridden = 0,
				.isSunEventDriven = 0
			},
			.autoOpenTime =
			{
				.hour = anyByteLessThan(24),
				.minute = anyByteLessThan(60)
			},
			.autoCloseTime =
			{
				.hour = anyByteLessThan(24),
				.minute = anyByteLessThan(60)
			}
		}
	};

	stubNvmApplicationSettings(&settings);
}

void publishDateChanged(void)
{
	static const struct DateWithFlags today = { };
	static const struct DateChanged dateChangedEventArgs = { .today = &today };
	eventPublish(DATE_CHANGED, &dateChangedEventArgs);
}

void assertFarSchedulesAreEqualWithAnyNonNullArgs(
	const struct FarSchedule *const expected,
	const struct FarSchedule *const actual)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(expected, "NULL1");
	TEST_ASSERT_NOT_NULL_MESSAGE(actual, "NULL2");
	TEST_ASSERT_NOT_NULL_MESSAGE(actual->eventArgs, "NULL3");

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->time.hour,
		actual->time.hour,
		"Hour");

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->time.minute,
		actual->time.minute,
		"Minute");

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->eventType,
		actual->eventType,
		"Event");
}

void farSchedulerAdd(const struct FarSchedule *const schedule)
{
	farSchedulerAddArgs[farSchedulerAddCalls++ & 7] = schedule;
}

void stubNvmSettingsForManuallyDrivenMode(void)
{
	union ApplicationNvmSettings settings =
	{
		.door =
		{
			.mode =
			{
				.isTimeDriven = 0,
				.isManuallyOverridden = 1,
				.isSunEventDriven = 0
			}
		}
	};

	stubNvmApplicationSettings(&settings);
}

void stubNvmSettingsForSunEventDrivenMode(void)
{
	union ApplicationNvmSettings settings =
	{
		.door =
		{
			.mode =
			{
				.isTimeDriven = 0,
				.isManuallyOverridden = 0,
				.isSunEventDriven = 1
			}
		}
	};

	stubNvmApplicationSettings(&settings);
}

void stubNvmSettingsForUnspecifiedMode(void)
{
	union ApplicationNvmSettings settings =
	{
		.door =
		{
			.mode =
			{
				.isTimeDriven = 0,
				.isManuallyOverridden = 0,
				.isSunEventDriven = 0
			}
		}
	};

	stubNvmApplicationSettings(&settings);
}
