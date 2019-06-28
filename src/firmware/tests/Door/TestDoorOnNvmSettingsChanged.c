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

void onBeforeTest(void)
{
	doorFixtureInitialise();
}

void onAfterTest(void)
{
	doorFixtureShutdown();
}

void test_nvmSettingsChanged_onPublishedWhenDoorIsTimeDriven_expectScheduleIsAddedForOpeningTimeStoredInNvm(void)
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

	publishNvmSettingsChanged();
	dispatchAllEvents();

	TEST_ASSERT_TRUE(farSchedulerAddCalls >= 1);
	assertFarSchedulesAreEqualWithAnyNonNullArgs(
		&expectedOpeningSchedule,
		farSchedulerAddArgs[0]);
}

void test_nvmSettingsChanged_onPublishedWhenDoorIsTimeDriven_expectPreviousScheduleIsRemovedBeforeAddingNewDoorOpeningTime(void)
{
	stubNvmSettingsForTimeDrivenMode();
	publishNvmSettingsChanged();
	dispatchAllEvents();

	TEST_ASSERT_TRUE_MESSAGE(farSchedulerRemoveCalls >= 1, "Calls");
	TEST_ASSERT_EQUAL(farSchedulerAddArgs[0], farSchedulerRemoveArgs[0]);
	TEST_ASSERT_TRUE(farSchedulerRemoveSequence[0] < farSchedulerAddSequence[0]);
}

void test_nvmSettingsChanged_onPublishedWhenDoorIsTimeDriven_expectScheduleIsAddedForClosingTimeStoredInNvm(void)
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

	publishNvmSettingsChanged();
	dispatchAllEvents();

	TEST_ASSERT_TRUE(farSchedulerAddCalls >= 2);
	assertFarSchedulesAreEqualWithAnyNonNullArgs(
		&expectedClosingSchedule,
		farSchedulerAddArgs[1]);
}

void test_nvmSettingsChanged_onPublishedWhenDoorIsTimeDriven_expectPreviousScheduleIsRemovedBeforeAddingNewDoorClosingTime(void)
{
	stubNvmSettingsForTimeDrivenMode();
	publishNvmSettingsChanged();
	dispatchAllEvents();

	TEST_ASSERT_TRUE_MESSAGE(farSchedulerRemoveCalls >= 2, "Calls");
	TEST_ASSERT_EQUAL(farSchedulerAddArgs[1], farSchedulerRemoveArgs[1]);
	TEST_ASSERT_TRUE(farSchedulerRemoveSequence[1] < farSchedulerAddSequence[1]);
}

void test_nvmSettingsChanged_onPublishedWhenDoorIsManuallyDriven_expectSchedulesAreNotAddedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForManuallyDrivenMode();
	publishNvmSettingsChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, farSchedulerAddCalls);
}

void test_nvmSettingsChanged_onPublishedWhenDoorIsManuallyDriven_expectSchedulesAreRemovedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForManuallyDrivenMode();
	publishNvmSettingsChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(2, farSchedulerRemoveCalls);
	TEST_ASSERT_NOT_NULL_MESSAGE(farSchedulerAddArgs[0], "Open");
	TEST_ASSERT_NOT_NULL_MESSAGE(farSchedulerAddArgs[1], "Close");
}

void test_nvmSettingsChanged_onPublishedWhenDoorIsSunEventDriven_expectSchedulesAreNotAddedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForSunEventDrivenMode();
	publishNvmSettingsChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, farSchedulerAddCalls);
}

void test_nvmSettingsChanged_onPublishedWhenDoorIsSunEventDriven_expectSchedulesAreRemovedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForSunEventDrivenMode();
	publishNvmSettingsChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(2, farSchedulerRemoveCalls);
	TEST_ASSERT_NOT_NULL_MESSAGE(farSchedulerAddArgs[0], "Open");
	TEST_ASSERT_NOT_NULL_MESSAGE(farSchedulerAddArgs[1], "Close");
}

void test_nvmSettingsChanged_onPublishedWhenDoorIsUnspecifiedMode_expectSchedulesAreNotAddedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForUnspecifiedMode();
	publishNvmSettingsChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, farSchedulerAddCalls);
}

void test_nvmSettingsChanged_onPublishedWhenDoorIsUnspecifiedMode_expectSchedulesAreRemovedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForUnspecifiedMode();
	publishNvmSettingsChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(2, farSchedulerRemoveCalls);
	TEST_ASSERT_NOT_NULL_MESSAGE(farSchedulerAddArgs[0], "Open");
	TEST_ASSERT_NOT_NULL_MESSAGE(farSchedulerAddArgs[1], "Close");
}
