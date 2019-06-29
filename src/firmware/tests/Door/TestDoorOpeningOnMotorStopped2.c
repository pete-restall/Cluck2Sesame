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

void test_motorStopped_onPublishedWithCurrentUnknownFault_expectDoorAbortedIsPublishedWithNoFaultFlags(void)
{
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_Opening, anyTransition);

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

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsOpeningAndTransitionIsOpen_expectMotorIsDisabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opening,
		.transition = DoorTransition_Open
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsOpeningAndTransitionIsUnchanged_expectMotorIsDisabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opening,
		.transition = DoorTransition_Unchanged
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsOpeningAndTransitionIsClose_expectMotorIsNotDisabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opening,
		.transition = DoorTransition_Close
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, motorDisableCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsOpeningAndTransitionIsOpen_expectMotorIsDisabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opening,
		.transition = DoorTransition_Open
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsOpeningAndTransitionIsUnchanged_expectMotorIsDisabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opening,
		.transition = DoorTransition_Unchanged
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsOpeningAndTransitionIsClose_expectMotorIsDisabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opening,
		.transition = DoorTransition_Close
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}
