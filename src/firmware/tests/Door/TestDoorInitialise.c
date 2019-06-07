#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "NvmSettings.h"
#include "ApplicationNvmSettings.h"
#include "Door.h"

#include "DoorFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"
#include "../NvmSettingsFixture.h"

TEST_FILE("Door/DoorInitialise.c")

static void stubNvmSettingsForTimeDrivenMode(void);
static void assertFarSchedulesAreEqualWithAnyNonNullArgs(
	const struct FarSchedule *const expected,
	const struct FarSchedule *const actual);

static uint8_t farSchedulerAddCalls;
static const struct FarSchedule *farSchedulerAddArgs[8];

void onBeforeTest(void)
{
	farSchedulerAddCalls = 0;
}

void onAfterTest(void)
{
}

void test_doorInitialise_calledWhenDoorIsTimeDriven_expectScheduleIsAddedForOpeningTimeStoredInNvm(void)
{
	stubNvmSettingsForTimeDrivenMode();
	struct FarSchedule expectedOpeningSchedule =
	{
		.time =
		{
			.hour = nvmSettings.application.door.autoOpenTime.hour,
			.minute = nvmSettings.application.door.autoOpenTime.minute
		},
		.eventType = DOOR_OPEN_SCHEDULE_ACTIONED
	};

	doorInitialise();

	TEST_ASSERT_TRUE(farSchedulerAddCalls >= 1);
	assertFarSchedulesAreEqualWithAnyNonNullArgs(
		&expectedOpeningSchedule,
		farSchedulerAddArgs[0]);
}

static void stubNvmSettingsForTimeDrivenMode(void)
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

static void assertFarSchedulesAreEqualWithAnyNonNullArgs(
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
