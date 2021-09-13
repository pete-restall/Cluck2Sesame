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
TEST_FILE("Door/DoorCalibrate.c")
TEST_FILE("Door/DoorOnAborted.c")
TEST_FILE("Door/DoorOnOpenScheduleActioned.c")
TEST_FILE("Door/DoorOnCloseScheduleActioned.c")
TEST_FILE("Door/DoorOnMotorStopped.c")
TEST_FILE("Door/DoorOnMotorEnabled.c")

void test_findBottomAfterLoweringStop_expectMotorIsRaisedAbout1m(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	motorOnCalls = 0;
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorOnCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(PULSES_PER_1M, motorOnArgs[0], "Arg");
}

void test_findBottomAfterLoweringStopHasFaults_expectMotorIsNotTurnedOn(void)
{
	enterFindBottomState();
	publishMotorStoppedWithFaultsOnLowering();
	motorOnCalls = 0;
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorOnCalls, "Calls");
}

void test_findBottomWhenRaisingStopHasNoFaults_expectFaultStateWithUnmodifiedTransition(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_FindBottom, anyTransition);
	publishMotorStoppedWithNoFaultsOnRaising();
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Fault, state.current, "S");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(anyTransition, state.transition, "T");
}

void test_findBottomWhenRaisingStopHasNoFaults_expectMotorIsDisabled(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	motorDisableCalls = 0;
	publishMotorStoppedWithNoFaultsOnRaising();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_findBottomWhenRaisingStopHasNoFaults_expectMotorIsNotTurnedOn(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	motorOnCalls = 0;
	publishMotorStoppedWithNoFaultsOnRaising();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_findBottomAfterRaisingStopHasNoFaults_expectDoorAbortedIsPublishedWithLineSnappedFlag(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	publishMotorStoppedWithNoFaultsOnRaising();
	mockOnDoorAborted();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onDoorAbortedCalls, "Calls");
	TEST_ASSERT_NOT_NULL(onDoorAbortedArgs[0]);
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isJammed, "J");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isReversed, "R");
	TEST_ASSERT_TRUE_MESSAGE(onDoorAbortedArgs[0]->fault.isLineSnapped, "S");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isLineTooLong, "L");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isEncoderBroken, "E");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isInsufficientPower, "P");
}

void test_findBottomWhenRaisingStopHasCurrentLimitFault_expectFindBottomStateWithUnmodifiedTransition(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_FindBottom, anyTransition);
	publishMotorStopped(&raisingStoppedImmediately);
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_FindBottom, state.current, "S");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(anyTransition, state.transition, "T");
}

void test_findBottomWhenRaisingStopHasCurrentLimitFault_expectMotorIsNotDisabled(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	publishMotorStopped(&raisingStoppedImmediately);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorDisableCalls);
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsAbout2mm_expectMotorIsLoweredAgain(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	struct MotorStopped stoppedAlmostImmediately =
	{
		.requestedCount = 1234,
		.actualCount = PULSES_PER_2MM,
		.fault = { .currentLimited = 1 }
	};

	motorOnCalls = 0;
	publishMotorStopped(&stoppedAlmostImmediately);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorOnCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(-PULSES_PER_10CM, motorOnArgs[0], "Arg");
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsLessThan2mm_expectMotorIsLoweredAgain(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	struct MotorStopped stoppedAlmostImmediately =
	{
		.requestedCount = 1234,
		.actualCount = (int16_t) anyWordLessThan(PULSES_PER_2MM),
		.fault = { .currentLimited = 1 }
	};

	motorOnCalls = 0;
	publishMotorStopped(&stoppedAlmostImmediately);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorOnCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(-PULSES_PER_10CM, motorOnArgs[0], "Arg");
}

void test_findBottomWhenRaisingStopHasCurrentLimitFaultAndActualCountIsZero_expectMotorIsLoweredAgain(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	motorOnCalls = 0;
	publishMotorStopped(&raisingStoppedImmediately);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorOnCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(-PULSES_PER_10CM, motorOnArgs[0], "Arg");
}
