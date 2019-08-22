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

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsClosingWaitingForEnabledMotor_expectUnknownStateWithUnmodifiedTransition(void)
{
	uint8_t initialTransition = anyByte();
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closing_WaitingForEnabledMotor,
		.transition = initialTransition
	};

	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Unknown, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(initialTransition, state.transition, "T");
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsClosingWaitingForEnabledMotor_expectUnknownStateWithUnmodifiedTransition(void)
{
	uint8_t initialTransition = anyByte();
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closing_WaitingForEnabledMotor,
		.transition = initialTransition
	};

	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Unknown, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(initialTransition, state.transition, "T");
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsClosingWaitingForEnabledMotorAndMotorIsDisabled_expectMotorIsDisabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closing_WaitingForEnabledMotor,
		.transition = anyByte()
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsClosingWaitingForEnabledMotorAndMotorIsEnabled_expectMotorIsDisabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closing_WaitingForEnabledMotor,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsClosingWaitingForEnabledMotorAndMotorIsDisabled_expectMotorIsDisabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closing_WaitingForEnabledMotor,
		.transition = anyByte()
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsClosingWaitingForEnabledMotorAndMotorIsEnabled_expectMotorIsDisabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closing_WaitingForEnabledMotor,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}
