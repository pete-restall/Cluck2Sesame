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
TEST_FILE("Door/DoorCalibrate.c")
TEST_FILE("Door/DoorOnAborted.c")
TEST_FILE("Door/DoorOnOpenScheduleActioned.c")
TEST_FILE("Door/DoorOnCloseScheduleActioned.c")
TEST_FILE("Door/DoorOnMotorStopped.c")
TEST_FILE("Door/DoorOnMotorEnabled.c")

static void publishAnySunEventsChanged(void);

void onBeforeTest(void)
{
	doorFixtureInitialise();
}

void onAfterTest(void)
{
	doorFixtureShutdown();
}

void test_sunEventsChanged_onPublishedWhenDoorIsSunEventDriven_expectScheduleIsAddedForOpeningTimeOfSunrise(void)
{
	stubNvmSettingsForSunEventDrivenModeWithOffsets(0, 0);

	struct SunEventsChanged sunEvents;
	stubAnySunEvents(&sunEvents);

	struct FarSchedule expectedOpeningSchedule =
	{
		.time =
		{
			.hour = sunEvents.sunrise.hour,
			.minute = sunEvents.sunrise.minute
		},
		.eventType = DOOR_OPEN_SCHEDULE_ACTIONED
	};

	publishSunEventsChanged(&sunEvents);
	dispatchAllEvents();

	TEST_ASSERT_TRUE(farSchedulerAddCalls >= 1);
	assertFarSchedulesAreEqualWithAnyNonNullArgs(
		&expectedOpeningSchedule,
		farSchedulerAddArgs[0]);
}

void test_sunEventsChanged_onPublishedWhenDoorIsSunEventDriven_expectPreviousScheduleIsRemovedBeforeAddingNewDoorOpeningTime(void)
{
	stubNvmSettingsForSunEventDrivenMode();
	publishAnySunEventsChanged();
	dispatchAllEvents();

	TEST_ASSERT_TRUE_MESSAGE(farSchedulerRemoveCalls >= 1, "Calls");
	TEST_ASSERT_EQUAL(farSchedulerAddArgs[0], farSchedulerRemoveArgs[0]);
	TEST_ASSERT_TRUE(farSchedulerRemoveSequence[0] < farSchedulerAddSequence[0]);
}

static void publishAnySunEventsChanged(void)
{
	struct SunEventsChanged sunEvents;
	stubAnySunEvents(&sunEvents);
	publishSunEventsChanged(&sunEvents);
}

void test_sunEventsChanged_onPublishedWhenDoorIsSunEventDriven_expectScheduleIsAddedForClosingTimeOfSunset(void)
{
	stubNvmSettingsForSunEventDrivenModeWithOffsets(0, 0);

	struct SunEventsChanged sunEvents;
	stubAnySunEvents(&sunEvents);

	struct FarSchedule expectedClosingSchedule =
	{
		.time =
		{
			.hour = sunEvents.sunset.hour,
			.minute = sunEvents.sunset.minute
		},
		.eventType = DOOR_CLOSE_SCHEDULE_ACTIONED
	};

	publishSunEventsChanged(&sunEvents);
	dispatchAllEvents();

	TEST_ASSERT_TRUE(farSchedulerAddCalls >= 2);
	assertFarSchedulesAreEqualWithAnyNonNullArgs(
		&expectedClosingSchedule,
		farSchedulerAddArgs[1]);
}

void test_sunEventsChanged_onPublishedWhenDoorIsSunEventDriven_expectPreviousScheduleIsRemovedBeforeAddingNewDoorClosingTime(void)
{
	stubNvmSettingsForSunEventDrivenMode();
	publishAnySunEventsChanged();
	dispatchAllEvents();

	TEST_ASSERT_TRUE_MESSAGE(farSchedulerRemoveCalls >= 2, "Calls");
	TEST_ASSERT_EQUAL(farSchedulerAddArgs[1], farSchedulerRemoveArgs[1]);
	TEST_ASSERT_TRUE(farSchedulerRemoveSequence[1] < farSchedulerAddSequence[1]);
}

void test_sunEventsChanged_onPublishedWhenDoorIsManuallyDriven_expectSchedulesAreNotAddedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForManuallyDrivenMode();
	publishAnySunEventsChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, farSchedulerAddCalls);
}

void test_sunEventsChanged_onPublishedWhenDoorIsManuallyDriven_expectSchedulesAreNotRemovedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForManuallyDrivenMode();
	publishAnySunEventsChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, farSchedulerRemoveCalls);
}

void test_sunEventsChanged_onPublishedWhenDoorIsTimeDriven_expectSchedulesAreNotAddedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForTimeDrivenMode();
	publishAnySunEventsChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, farSchedulerAddCalls);
}

void test_sunEventsChanged_onPublishedWhenDoorIsTimeDriven_expectSchedulesAreNotRemovedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForTimeDrivenMode();
	publishAnySunEventsChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, farSchedulerRemoveCalls);
}

void test_sunEventsChanged_onPublishedWhenDoorIsUnspecifiedMode_expectSchedulesAreNotAddedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForUnspecifiedMode();
	publishAnySunEventsChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, farSchedulerAddCalls);
}

void test_sunEventsChanged_onPublishedWhenDoorIsUnspecifiedMode_expectSchedulesAreNotRemovedForOpeningAndClosingTimes(void)
{
	stubNvmSettingsForUnspecifiedMode();
	publishAnySunEventsChanged();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, farSchedulerRemoveCalls);
}

void test_sunEventsChanged_onPublished_expectDoorOpeningSchedulePointerIsSharedAmongstModes(void)
{
	stubNvmSettingsForTimeDrivenMode();
	publishDateChanged();
	dispatchAllEvents();
	stubNvmSettingsForSunEventDrivenMode();
	publishAnySunEventsChanged();
	dispatchAllEvents();
	TEST_ASSERT_TRUE_MESSAGE(farSchedulerAddCalls >= 4, "Add");
	TEST_ASSERT_TRUE_MESSAGE(farSchedulerAddArgs[0] == farSchedulerAddArgs[2], "Ptr Add");

	TEST_ASSERT_TRUE_MESSAGE(farSchedulerRemoveCalls >= 4, "Remove");
	TEST_ASSERT_TRUE_MESSAGE(farSchedulerRemoveArgs[0] == farSchedulerRemoveArgs[2], "Ptr Remove");
}

void test_sunEventsChanged_onPublished_expectDoorClosingSchedulePointerIsSharedAmongstModes(void)
{
	stubNvmSettingsForTimeDrivenMode();
	publishDateChanged();
	dispatchAllEvents();
	stubNvmSettingsForSunEventDrivenMode();
	publishAnySunEventsChanged();
	dispatchAllEvents();
	TEST_ASSERT_TRUE_MESSAGE(farSchedulerAddCalls >= 4, "Add");
	TEST_ASSERT_TRUE_MESSAGE(farSchedulerAddArgs[1] == farSchedulerAddArgs[1], "Ptr Add");

	TEST_ASSERT_TRUE_MESSAGE(farSchedulerRemoveCalls >= 4, "Remove");
	TEST_ASSERT_TRUE_MESSAGE(farSchedulerRemoveArgs[3] == farSchedulerRemoveArgs[3], "Ptr Remove");
}
