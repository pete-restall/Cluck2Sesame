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

void onBeforeTest(void)
{
	doorFixtureInitialise();
}

void onAfterTest(void)
{
	doorFixtureShutdown();
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

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsClosingAndTransitionIsClose_expectMotorIsDisabled(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Close);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsClosingAndTransitionIsUnchanged_expectMotorIsDisabled(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Unchanged);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsClosingAndTransitionIsOpen_expectMotorIsNotDisabled(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Open);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorDisableCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsClosingAndTransitionIsClosexpectMotorIsDisabled(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Close);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsClosingAndTransitionIsUnchanged_expectMotorIsDisabled(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Unchanged);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsClosingAndTransitionIsOpen_expectMotorIsDisabled(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Open);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsClosingAndTransitionIsClose_expectMotorCurrentLimitIsUnchanged(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Close);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsNoLoadCalls);
	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsMaximumLoadCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsClosingAndTransitionIsUnchanged_expectMotorCurrentLimitIsUnchanged(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Unchanged);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsNoLoadCalls);
	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsMaximumLoadCalls);
}

void test_motorStopped_onPublishedWthNoFaultsWhenStateIsClosingAndTransitionIsOpen_expectMotorCurrentLimitIsMaximumLoad(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Open);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsNoLoadCalls);
	TEST_ASSERT_EQUAL_UINT8(1, motorLimitIsMaximumLoadCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsClosingAndTransitionIsClose_expectMotorCurrentLimitIsUnchanged(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Close);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsNoLoadCalls);
	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsMaximumLoadCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsClosingAndTransitionIsUnchanged_expectMotorCurrentLimitIsUnchanged(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Unchanged);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsNoLoadCalls);
	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsMaximumLoadCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsClosingAndTransitionIsOpen_expectMotorCurrentLimitIsUnchanged(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Open);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsNoLoadCalls);
	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsMaximumLoadCalls);
}
