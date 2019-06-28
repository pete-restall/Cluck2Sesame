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

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsFault_expectStateAndTransitionAreUnmodified(void)
{
	uint8_t initialTransition = anyByte();
	struct DoorStateWithContext state =
	{
		.current = DoorState_Fault,
		.transition = initialTransition
	};

	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Fault, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(initialTransition, state.transition, "T");
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsFault_expectStateAndTransitionAreUnmodified(void)
{
	uint8_t initialTransition = anyByte();
	struct DoorStateWithContext state =
	{
		.current = DoorState_Fault,
		.transition = initialTransition
	};

	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Fault, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(initialTransition, state.transition, "T");
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsFaultAndMotorIsEnabled_expectMotorIsNotDisabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Fault,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, motorDisableCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsFaultAndMotorIsEnabled_expectMotorIsNotDisabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Fault,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, motorDisableCalls);
}