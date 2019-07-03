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

#define PULSES_PER_10CM 1576

static void enterFindBottomState(void);

void onBeforeTest(void)
{
	doorFixtureInitialise();
}

void onAfterTest(void)
{
	doorFixtureShutdown();
}

void test_findBottom_expectDoorIsFirstLoweredAbout10cm(void)
{
	enterFindBottomState();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorOnCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(motorOnArgs[0], -PULSES_PER_10CM, "Arg");
}

static void enterFindBottomState(void)
{
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_Unknown, anyTransition);
	stubMotorIsEnabled();
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);
	TEST_ASSERT_EQUAL_UINT8(DoorState_FindBottom, state.current);
}

void test_findBottomAfterFirstStop_expectMotorIsNotDisabled(void)
{
	enterFindBottomState();
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorDisableCalls);
}

void test_findBottomWhenFirstStopHasFaults_expectFaultStateWithUnmodifiedTransition(void)
{
	enterFindBottomState();
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_FindBottom, anyTransition);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Fault, state.current, "S");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(anyTransition, state.transition, "T");
}

void test_findBottomWhenFirstStopHasFaults_expectMotorIsDisabled(void)
{
	enterFindBottomState();
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_findBottomWhenFirstStopCurrentLimitFault_expectMotorDoorAbortedIsPublishedWithReversedFlag(void)
{
	enterFindBottomState();

	static const struct MotorStopped reversed =
	{
		.actualCount = 123,
		.requestedCount = 456,
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

void test_findBottomWhenFirstStopEncoderOverflowFault_expectMotorDoorAbortedIsPublishedWithLineTooLongFlag(void)
{
	enterFindBottomState();

	static const struct MotorStopped tooLong =
	{
		.actualCount = 123,
		.requestedCount = 456,
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

void test_findBottomWhenFirstStopEncoderTimeoutFault_expectMotorDoorAbortedIsPublishedWithEncoderBrokenFlag(void)
{
	enterFindBottomState();

	static const struct MotorStopped timeout =
	{
		.actualCount = 123,
		.requestedCount = 456,
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

void test_findBottomWhenFirstStopUnknownFault_expectDoorAbortedIsPublishedWithNoFaultFlags(void)
{
	enterFindBottomState();

	struct MotorStopped unknown =
	{
		.actualCount = 123,
		.requestedCount = 456,
		.fault = { .all = anyByteWithMaskClear(0b00000111) }
	};

	publishMotorStopped(&unknown);
	mockOnDoorAborted();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onDoorAbortedCalls, "Calls");
	TEST_ASSERT_NOT_NULL(onDoorAbortedArgs[0]);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, onDoorAbortedArgs[0]->fault.all, "F");
}
