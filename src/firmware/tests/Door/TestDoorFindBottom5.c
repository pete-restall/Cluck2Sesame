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

void test_findBottomWhenRaisingStopHasEncoderNonCurrentLimitFault_expectFaultStateWithUnmodifiedTransition(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_FindBottom, anyTransition);
	publishMotorStoppedWithNonCurrentLimitFaultOnRaising();
	mockOnDoorAborted();
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Fault, state.current, "S");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(anyTransition, state.transition, "T");
}

void test_findBottomWhenRaisingStopHasNonCurrentLimitFault_expectMotorIsDisabled(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	motorDisableCalls = 0;
	publishMotorStoppedWithNonCurrentLimitFaultOnRaising();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_findBottomWhenRaisingStopHasNonCurrentLimitFault_expectMotorIsNotTurnedOn(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	motorOnCalls = 0;
	publishMotorStoppedWithNonCurrentLimitFaultOnRaising();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_findBottomAfterRaisingStopWithEncoderOverflowFault_expectDoorAbortedIsPublishedWithLineSnappedFlag(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	static const struct MotorStopped encoderOverflow =
	{
		.actualCount = 123,
		.requestedCount = 456,
		.fault = { .encoderOverflow = 1 }
	};

	publishMotorStopped(&encoderOverflow);
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

void test_findBottomAfterRaisingStopWithEncoderTimeoutFault_expectDoorAbortedIsPublishedWithEncoderBrokenFlag(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	static const struct MotorStopped encoderTimeout =
	{
		.actualCount = 123,
		.requestedCount = 456,
		.fault = { .encoderTimeout = 1 }
	};

	publishMotorStopped(&encoderTimeout);
	mockOnDoorAborted();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onDoorAbortedCalls, "Calls");
	TEST_ASSERT_NOT_NULL(onDoorAbortedArgs[0]);
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isJammed, "J");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isReversed, "R");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isLineSnapped, "S");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isLineTooLong, "L");
	TEST_ASSERT_TRUE_MESSAGE(onDoorAbortedArgs[0]->fault.isEncoderBroken, "E");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isInsufficientPower, "P");
}

void test_findBottomAfterRaisingStopWithUnknownFault_expectDoorAbortedIsPublishedWithNoFlags(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();

	struct MotorStopped unknownFault =
	{
		.actualCount = 123,
		.requestedCount = 456,
		.fault = { .all = anyUnknownMotorFault() }
	};

	publishMotorStopped(&unknownFault);
	mockOnDoorAborted();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onDoorAbortedCalls, "Calls");
	TEST_ASSERT_NOT_NULL(onDoorAbortedArgs[0]);
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isJammed, "J");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isReversed, "R");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isLineSnapped, "S");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isLineTooLong, "L");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isEncoderBroken, "E");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isInsufficientPower, "P");
}
