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

TEST_FILE("Door/DoorInitialise.c")

static void stubNvmSettingsForTimeDrivenMode(void);
static void publishDateChanged(void);
static void assertFarSchedulesAreEqualWithAnyNonNullArgs(
	const struct FarSchedule *const expected,
	const struct FarSchedule *const actual);

static void stubNvmSettingsForManuallyDrivenMode(void);
static void stubNvmSettingsForSunEventDrivenMode(void);
static void stubNvmSettingsForUnspecifiedMode(void);

static uint8_t farSchedulerAddCalls;
static const struct FarSchedule *farSchedulerAddArgs[8];

void onBeforeTest(void)
{
	farSchedulerAddCalls = 0;
	eventInitialise();
	doorInitialise();
}

void onAfterTest(void)
{
}

void test_dateChanged_onPublishedWhenDoorIsTimeDriven_expectScheduleIsAddedForOpeningTimeStoredInNvm(void)
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

	publishDateChanged();
	dispatchAllEvents();

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

static void publishDateChanged(void)
{
	static const struct DateWithFlags today = { };
	static const struct DateChanged dateChangedEventArgs = { .today = &today };
	eventPublish(DATE_CHANGED, &dateChangedEventArgs);
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

void test_dateChanged_onPublishedWhenDoorIsTimeDriven_expectScheduleIsAddedForClosingTimeStoredInNvm(void)
{
	stubNvmSettingsForTimeDrivenMode();
	struct FarSchedule expectedClosingSchedule =
	{
		.time =
		{
			.hour = nvmSettings.application.door.autoCloseTime.hour,
			.minute = nvmSettings.application.door.autoCloseTime.minute
		},
		.eventType = DOOR_CLOSE_SCHEDULE_ACTIONED
	};

	publishDateChanged();
	dispatchAllEvents();

	TEST_ASSERT_TRUE(farSchedulerAddCalls >= 2);
	assertFarSchedulesAreEqualWithAnyNonNullArgs(
		&expectedClosingSchedule,
		farSchedulerAddArgs[1]);
}

void test_dateChanged_onPublishedWhenDoorIsManuallyDriven_expectScheduleIsNotAddedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForManuallyDrivenMode();
	publishDateChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, farSchedulerAddCalls);
}

static void stubNvmSettingsForManuallyDrivenMode(void)
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

void test_dateChanged_onPublishedWhenDoorIsSunEventDriven_expectScheduleIsNotAddedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForSunEventDrivenMode();
	publishDateChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, farSchedulerAddCalls);
}

static void stubNvmSettingsForSunEventDrivenMode(void)
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

void test_dateChanged_onPublishedWhenDoorIsUnspecifiedMode_expectScheduleIsNotAddedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForUnspecifiedMode();
	publishDateChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, farSchedulerAddCalls);
}

static void stubNvmSettingsForUnspecifiedMode(void)
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
