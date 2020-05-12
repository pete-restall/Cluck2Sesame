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
TEST_FILE("Door/DoorOnMotorEnabled.c")

void onBeforeTest(void)
{
	doorFixtureInitialise();
}

void onAfterTest(void)
{
	doorFixtureShutdown();
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsClosingAndTransitionIsClose_expectMotorIsNotTurnedOn(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Close);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsClosingAndTransitionIsUnchanged_expectMotorIsNotTurnedOn(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Unchanged);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsClosingAndTransitionIsOpen_expectMotorIsTurnedOnAfterCurrentLimitIsSet(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Open);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorOnCalls, "Calls");
	TEST_ASSERT_TRUE_MESSAGE(motorOnSequence > motorLimitIsMaximumLoadSequence, "Seq");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(nvmSettings.application.door.height, motorOnArgs[0], "Arg");
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsClosingAndTransitionIsClose_expectMotorIsNotTurnedOn(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Close);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsClosingAndTransitionIsUnchanged_expectMotorIsNotTurnedOn(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Unchanged);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsClosingAndTransitionIsOpen_expectMotorIsNotTurnedOn(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Closing, DoorTransition_Open);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsClosingAndTransitionIsOpen_expectStateIsOpeningWithOpenTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closing,
		.transition = DoorTransition_Open
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Opening, state.current, "S");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsClosingAndTransitionIsOpen_expectStateIsFaultWithOpenTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closing,
		.transition = DoorTransition_Open
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Fault, state.current, "S");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}
