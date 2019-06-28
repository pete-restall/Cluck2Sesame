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

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsOpeningAndTransitionIsOpen_expectOpenedStateWithUnchangedTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opening,
		.transition = DoorTransition_Open
	};

	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Opened, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Unchanged, state.transition, "T");
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsOpeningAndTransitionIsUnchanged_expectOpenedStateWithUnchangedTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opening,
		.transition = DoorTransition_Unchanged
	};

	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Opened, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Unchanged, state.transition, "T");
}

// TODO: NO FAULTS WHEN TRANSITION IS CLOSE; WE NEED TO SET THE CURRENT LIMIT TO NO-LOAD, TURN THE MOTOR ON AND SET THE STATE TO DoorState_Closing

void test_motorStopped_onPublishedWithFaultsWhenStateIsOpening_expectFaultStateWithUnmodifiedTransition(void)
{
	uint8_t initialTransition = anyByte();
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opening,
		.transition = initialTransition
	};

	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Fault, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(initialTransition, state.transition, "T");
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
