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
TEST_FILE("Door/DoorGetState.c")
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

void test_motorStopped_onPublishedWithNoFaultsAndTransitionOfClose_expectDoorClosedIsPublished(void)
{
	stubDoorWithState(DoorState_Closing, DoorTransition_Close);
	publishMotorStoppedWithNoFaults();
	mockOnDoorClosed();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onDoorClosedCalls, "Calls");
	TEST_ASSERT_NOT_NULL(onDoorClosedArgs[0]);
}

void test_motorStopped_onPublishedWithNoFaultsAndTransitionOfUnchanged_expectDoorClosedIsPublished(void)
{
	stubDoorWithState(DoorState_Closing, DoorTransition_Unchanged);
	publishMotorStoppedWithNoFaults();
	mockOnDoorClosed();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onDoorClosedCalls, "Calls");
	TEST_ASSERT_NOT_NULL(onDoorClosedArgs[0]);
}

void test_motorStopped_onPublishedWithNoFaultsAndTransitionOfOpen_expectDoorClosedIsPublished(void)
{
	stubDoorWithState(DoorState_Closing, DoorTransition_Open);
	publishMotorStoppedWithNoFaults();
	mockOnDoorClosed();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onDoorClosedCalls, "Calls");
	TEST_ASSERT_NOT_NULL(onDoorClosedArgs[0]);
}

void test_motorStopped_onPublishedWithNoFaults_expectDoorAbortedIsNotPublished(void)
{
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_Closing, anyTransition);

	publishMotorStoppedWithNoFaults();
	mockOnDoorAborted();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, onDoorAbortedCalls, "Calls");
}

void test_motorStopped_onPublishedWithFaults_expectDoorClosedIsNotPublished(void)
{
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_Closing, anyTransition);
	publishMotorStoppedWithFaults();
	mockOnDoorClosed();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, onDoorClosedCalls, "Calls");
}
