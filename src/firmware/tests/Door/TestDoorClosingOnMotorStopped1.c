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

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsClosingAndTransitionIsClose_expectClosedStateWithUnchangedTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closing,
		.transition = DoorTransition_Close
	};

	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Closed, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Unchanged, state.transition, "T");
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsClosingAndTransitionIsUnchanged_expectClosedStateWithUnchangedTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closing,
		.transition = DoorTransition_Unchanged
	};

	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Closed, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Unchanged, state.transition, "T");
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsClosing_expectFaultStateWithUnmodifiedTransition(void)
{
	uint8_t initialTransition = anyByte();
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closing,
		.transition = initialTransition
	};

	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Fault, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(initialTransition, state.transition, "T");
}

void test_motorStopped_onPublishedWithCurrentLimitFault_expectDoorAbortedIsPublishedWithReversedFlag(void)
{
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_Closing, anyTransition);

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

void test_motorStopped_onPublishedWithEncoderOverflowFault_expectDoorAbortedIsPublishedWithLineTooLongFlag(void)
{
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_Closing, anyTransition);

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

void test_motorStopped_onPublishedWithEncoderTimeoutFault_expectDoorAbortedIsPublishedWithEncoderBrokenFlag(void)
{
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_Closing, anyTransition);

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

void test_motorStopped_onPublishedWithUnknownFault_expectDoorAbortedIsPublishedWithNoFaultFlags(void)
{
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_Closing, anyTransition);

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
