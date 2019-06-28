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

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosedAndMotorIsEnabled_expectOpeningStateWithOpenTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closed,
		.transition = anyByteExcept(DoorTransition_Open)
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Opening, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosedAndMotorIsEnabled_expectMotorIsEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closed,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorEnableCalls);
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosedAndMotorIsEnabled_expectMotorIsTurnedOn(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closed,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorOnCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(
		nvmSettings.application.door.height,
		motorOnArgs[0],
		"Height");
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosedAndMotorIsEnabled_expectMotorCurrentLimitIsMaximumLoad(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closed,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsNoLoadCalls, "N");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorLimitIsMaximumLoadCalls, "M");
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosedAndMotorIsEnabled_expectMotorIsEnabledBeforeCurrentLimitIsChanged(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closed,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_TRUE(motorEnableSequence < motorLimitIsMaximumLoadSequence);
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosedAndMotorIsEnabled_expectMotorCurrentLimitIsChangedBeforeMotorIsTurnedOn(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closed,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_TRUE(motorLimitIsMaximumLoadSequence < motorOnSequence);
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosedAndMotorIsDisabled_expectOpeningWaitingForEnabledMotorStateWithOpenTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closed,
		.transition = anyByteExcept(DoorTransition_Open)
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Opening_WaitingForEnabledMotor, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosedAndMotorIsDisabled_expectMotorIsEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closed,
		.transition = anyByte()
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorEnableCalls);
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosedAndMotorIsDisabled_expectMotorIsNotTurnedOn(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closed,
		.transition = anyByte()
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosedAndMotorIsDisabled_expectMotorCurrentLimitIsNotChanged(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closed,
		.transition = anyByte()
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsNoLoadCalls, "N");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsMaximumLoadCalls, "M");
}
