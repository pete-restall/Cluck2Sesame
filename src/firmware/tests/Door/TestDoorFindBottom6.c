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

static void doMaximumFindBottomIterations(void);

void test_findBottomWhenRaisingLoweringSequenceHasRepeatedTenTimes_expectDoorAbortedIsPublishedWithLineTooLongFlag(void)
{
	mockOnDoorAborted();
	enterFindBottomState();
	doMaximumFindBottomIterations();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onDoorAbortedCalls, "Calls");
	TEST_ASSERT_NOT_NULL(onDoorAbortedArgs[0]);
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isJammed, "J");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isReversed, "R");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isLineSnapped, "S");
	TEST_ASSERT_TRUE_MESSAGE(onDoorAbortedArgs[0]->fault.isLineTooLong, "L");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isEncoderBroken, "E");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isInsufficientPower, "P");
}

static void doMaximumFindBottomIterations(void)
{
	for (uint8_t i = 0; i < 10; i++)
	{
		TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, onDoorAbortedCalls, "Calls");
		publishMotorStoppedWithNoFaultsOnLowering();
		publishMotorStopped(&raisingStoppedAtThreshold);
		dispatchAllEvents();
	}
}

void test_findBottomWhenRaisingLoweringSequenceHasRepeatedTenTimes_expectFaultStateWithUnmodifiedTransition(void)
{
	mockOnDoorAborted();
	enterFindBottomState();

	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_FindBottom, anyTransition);
	doMaximumFindBottomIterations();

	struct DoorStateWithContext state;
	doorGetState(&state);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Fault, state.current, "S");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(anyTransition, state.transition, "T");
}

void test_findBottomWhenRaisingLoweringSequenceHasRepeatedTenTimes_expectMotorIsDisabled(void)
{
	mockOnDoorAborted();
	enterFindBottomState();
	doMaximumFindBottomIterations();
	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_findBottomWhenRaisingLoweringSequenceHasRepeatedTenTimes_expectMotorIsNotTurnedOnForAnotherIteration(void)
{
	mockOnDoorAborted();
	enterFindBottomState();
	doMaximumFindBottomIterations();
	TEST_ASSERT_EQUAL_UINT8(2 * 10, motorOnCalls);
}

void test_findBottomWhenRaisingLoweringSequenceHasRepeatedTenTimes_expectFindBottomAlgorithmCanBeRepeated(void)
{
	mockOnDoorAborted();
	enterFindBottomState();
	doMaximumFindBottomIterations();

	struct DoorStateWithContext state =
	{
		.current = DoorState_Unknown,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Unknown, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_FindBottom, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Close, state.transition, "T");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(21, motorOnCalls, "ON");
}
