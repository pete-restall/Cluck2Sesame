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

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsOpeningAndTransitionIsOpen_expectMotorIsNotTurnedOn(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Open);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsOpeningAndTransitionIsUnchanged_expectMotorIsNotTurnedOn(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Unchanged);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsOpeningAndTransitionIsClose_expectMotorIsTurnedOnAfterCurrentLimitIsSet(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Close);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorOnCalls, "Calls");
	TEST_ASSERT_TRUE_MESSAGE(motorOnSequence > motorLimitIsNoLoadSequence, "Seq");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(-nvmSettings.application.door.height, motorOnArgs[0], "Arg");
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsOpeningAndTransitionIsOpen_expectMotorIsNotTurnedOn(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Open);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsOpeningAndTransitionIsUnchanged_expectMotorIsNotTurnedOn(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Unchanged);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsOpeningAndTransitionIsClose_expectMotorIsNotTurnedOn(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Close);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsOpeningAndTransitionIsClose_expectStateIsClosingWithCloseTransition(void)
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

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Closing, state.current, "S");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Close, state.transition, "T");
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsOpeningAndTransitionIsClose_expectStateIsFaultWithCloseTransition(void)
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

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Fault, state.current, "S");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Close, state.transition, "T");
}
