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
TEST_FILE("Door/DoorOnAborted.c")
TEST_FILE("Door/DoorOnOpenScheduleActioned.c")
TEST_FILE("Door/DoorOnCloseScheduleActioned.c")
TEST_FILE("Door/DoorOnMotorStopped.c")
TEST_FILE("Door/DoorOnMotorEnabled.c")

void onBeforeTest(void)
{
	doorFixtureInitialise();
}

void onAfterTest(void)
{
	doorFixtureShutdown();
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

void test_dateChanged_onPublishedWhenDoorIsManuallyDriven_expectSchedulesAreNotAddedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForManuallyDrivenMode();
	publishDateChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, farSchedulerAddCalls);
}

void test_dateChanged_onPublishedWhenDoorIsSunEventDriven_expectSchedulesAreNotAddedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForSunEventDrivenMode();
	publishDateChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, farSchedulerAddCalls);
}

void test_dateChanged_onPublishedWhenDoorIsUnspecifiedMode_expectSchedulesAreNotAddedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForUnspecifiedMode();
	publishDateChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, farSchedulerAddCalls);
}
