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

void test_findBottom_expectDoorIsFirstLoweredAbout10cm(void)
{
	enterFindBottomState();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorOnCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(motorOnArgs[0], -PULSES_PER_10CM, "Arg");
}

void test_findBottomAfterLoweringStop_expectMotorIsNotDisabled(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaultsOnLowering();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorDisableCalls);
}

void test_findBottomWhenLoweringStopHasFaults_expectFaultStateWithUnmodifiedTransition(void)
{
	enterFindBottomState();
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_FindBottom, anyTransition);
	publishMotorStoppedWithFaultsOnLowering();
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Fault, state.current, "S");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(anyTransition, state.transition, "T");
}

void test_findBottomWhenLoweringStopHasFaults_expectMotorIsDisabled(void)
{
	enterFindBottomState();
	publishMotorStoppedWithFaultsOnLowering();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_findBottomWhenLoweringStopCurrentLimitFault_expectDoorAbortedIsPublishedWithReversedFlag(void)
{
	enterFindBottomState();

	static const struct MotorStopped reversed =
	{
		.actualCount = -123,
		.requestedCount = -456,
		.fault = { .currentLimited = 1 }
	};

	publishMotorStopped(&reversed);
	mockOnDoorAborted();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onDoorAbortedCalls, "Calls");
	TEST_ASSERT_NOT_NULL(onDoorAbortedArgs[0]);
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isJammed, "J");
	TEST_ASSERT_TRUE_MESSAGE(onDoorAbortedArgs[0]->fault.isReversed, "R");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isLineSnapped, "S");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isLineTooLong, "L");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isEncoderBroken, "E");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isInsufficientPower, "P");
}

void test_findBottomWhenLoweringStopEncoderOverflowFault_expectDoorAbortedIsPublishedWithLineTooLongFlag(void)
{
	enterFindBottomState();

	static const struct MotorStopped tooLong =
	{
		.actualCount = -123,
		.requestedCount = -456,
		.fault = { .encoderOverflow = 1 }
	};

	publishMotorStopped(&tooLong);
	mockOnDoorAborted();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onDoorAbortedCalls, "Calls");
	TEST_ASSERT_NOT_NULL(onDoorAbortedArgs[0]);
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isJammed, "J");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isReversed, "R");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isLineSnapped, "S");
	TEST_ASSERT_TRUE_MESSAGE(onDoorAbortedArgs[0]->fault.isLineTooLong, "L");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isEncoderBroken, "E");
	TEST_ASSERT_FALSE_MESSAGE(onDoorAbortedArgs[0]->fault.isInsufficientPower, "P");
}

void test_findBottomWhenLoweringStopEncoderTimeoutFault_expectDoorAbortedIsPublishedWithEncoderBrokenFlag(void)
{
	enterFindBottomState();

	static const struct MotorStopped timeout =
	{
		.actualCount = -123,
		.requestedCount = -456,
		.fault = { .encoderTimeout = 1 }
	};

	publishMotorStopped(&timeout);
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

void test_findBottomWhenLoweringStopUnknownFault_expectDoorAbortedIsPublishedWithNoFaultFlags(void)
{
	enterFindBottomState();

	struct MotorStopped unknown =
	{
		.actualCount = -123,
		.requestedCount = -456,
		.fault = { .all = anyUnknownMotorFault() }
	};

	publishMotorStopped(&unknown);
	mockOnDoorAborted();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onDoorAbortedCalls, "Calls");
	TEST_ASSERT_NOT_NULL(onDoorAbortedArgs[0]);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, onDoorAbortedArgs[0]->fault.all, "F");
}
