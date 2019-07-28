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

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIs2mmOnCloseTransition_expectFindBottomStateAndUnmodifiedTransition(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_FindBottom, anyTransition);
	publishMotorStopped(&raisingStoppedAtThreshold);
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_FindBottom, state.current, "S");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(anyTransition, state.transition, "T");
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsMoreThan2mmOnCloseTransition_expectClosedStateAndUnchangedTransition(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	stubDoorWithState(DoorState_FindBottom, DoorTransition_Close);
	publishMotorStopped(&raisingStoppedAfterThreshold);
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Closed, state.current, "S");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Unchanged, state.transition, "T");
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsMoreThan2mmOnUnchangedTransition_expectClosedStateAndUnchangedTransition(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	stubDoorWithState(DoorState_FindBottom, DoorTransition_Unchanged);
	publishMotorStopped(&raisingStoppedAfterThreshold);
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Closed, state.current, "S");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Unchanged, state.transition, "T");
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsMoreThan2mmOnOpenTransition_expectOpeningStateAndOpenTransition(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	stubDoorWithState(DoorState_FindBottom, DoorTransition_Open);
	publishMotorStopped(&raisingStoppedAfterThreshold);
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Opening, state.current, "S");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsMoreThan2mmOnOpenTransition_expectMotorIsNotDisabled(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	stubDoorWithState(DoorState_FindBottom, DoorTransition_Open);
	publishMotorStopped(&raisingStoppedAfterThreshold);
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);
	TEST_ASSERT_EQUAL_UINT8(0, motorDisableCalls);
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsMoreThan2mmOnOpenTransition_expectMotorIsNotReEnabled(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	stubDoorWithState(DoorState_FindBottom, DoorTransition_Open);
	publishMotorStopped(&raisingStoppedAfterThreshold);
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);
	TEST_ASSERT_EQUAL_UINT8(1, motorEnableCalls);
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsMoreThan2mmOnOpenTransition_expectMotorCurrentLimitIsMaximum(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	motorLimitIsNoLoadCalls = 0;
	motorLimitIsMaximumLoadCalls = 0;
	stubDoorWithState(DoorState_FindBottom, DoorTransition_Open);
	publishMotorStopped(&raisingStoppedAfterThreshold);
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsNoLoadCalls, "N");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorLimitIsMaximumLoadCalls, "M");
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsMoreThan2mmOnOpenTransition_expectMotorCurrentLimitIsChangedBeforeMotorIsTurnedOn(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	motorLimitIsNoLoadCalls = 0;
	motorLimitIsMaximumLoadCalls = 0;
	stubDoorWithState(DoorState_FindBottom, DoorTransition_Open);
	publishMotorStopped(&raisingStoppedAfterThreshold);
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);

	TEST_ASSERT_TRUE(motorLimitIsMaximumLoadSequence < motorOnSequence);
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsMoreThan2mmOnOpenTransition_expectMotorIsTurnedOn(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	motorOnCalls = 0;
	stubDoorWithState(DoorState_FindBottom, DoorTransition_Open);
	publishMotorStopped(&raisingStoppedAfterThreshold);
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorOnCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(
		nvmSettings.application.door.height,
		motorOnArgs[0],
		"Height");
}
