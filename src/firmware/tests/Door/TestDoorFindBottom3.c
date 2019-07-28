#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/NvmSettings.h"
#include "ApplicationNvmSettings.h"
#include "Door.h"

#include "DoorFixture.h"
#include "DoorFindBottomFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"
#include "../NvmSettingsFixture.h"

TEST_FILE("Door/DoorInitialise.c")
TEST_FILE("Door/DoorGetState.c")
TEST_FILE("Door/DoorOnAborted.c")
TEST_FILE("Door/DoorOnOpenScheduleActioned.c")
TEST_FILE("Door/DoorOnCloseScheduleActioned.c")
TEST_FILE("Door/DoorOnMotorStopped.c")

void test_findBottomAfterRaisingStopHasCurrentLimitFault_expectDoorAbortedIsNotPublished(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	publishMotorStopped(&raisingStoppedImmediately);
	mockOnDoorAborted();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, onDoorAbortedCalls, "Calls");
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsJustMoreThan2mm_expectMotorIsNotLowered(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	motorOnCalls = 0;
	publishMotorStopped(&raisingStoppedJustAfterThreshold);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorOnCalls, "Calls");
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsMoreThan2mm_expectMotorIsNotLowered(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	motorOnCalls = 0;
	publishMotorStopped(&raisingStoppedJustAfterThreshold);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorOnCalls, "Calls");
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIs2mmOnCloseTransition_expectMotorIsNotDisabled(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	stubDoorWithState(DoorState_FindBottom, DoorTransition_Close);
	publishMotorStopped(&raisingStoppedAtThreshold);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorDisableCalls);
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsMoreThan2mmOnCloseTransition_expectMotorIsDisabled(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	motorDisableCalls = 0;
	stubDoorWithState(DoorState_FindBottom, DoorTransition_Close);
	publishMotorStopped(&raisingStoppedAfterThreshold);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorDisableCalls, "Calls");
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsMoreThan2mmOnUnchangedTransition_expectMotorIsDisabled(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	motorDisableCalls = 0;
	stubDoorWithState(DoorState_FindBottom, DoorTransition_Unchanged);
	publishMotorStopped(&raisingStoppedAfterThreshold);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorDisableCalls, "Calls");
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsMoreThan2mmOnOpenTransition_expectMotorIsNotDisabled(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	stubDoorWithState(DoorState_FindBottom, DoorTransition_Open);
	publishMotorStopped(&raisingStoppedAfterThreshold);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorDisableCalls, "Calls");
}
