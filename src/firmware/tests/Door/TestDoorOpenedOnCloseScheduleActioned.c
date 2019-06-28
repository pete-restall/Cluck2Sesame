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

void test_doorCloseScheduleActioned_onPublishedWhenStateIsOpenedAndMotorIsEnabled_expectClosingStateWithCloseTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opened,
		.transition = anyByteExcept(DoorTransition_Close)
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Closing, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Close, state.transition, "T");
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsOpenedAndMotorIsEnabled_expectMotorIsEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opened,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorEnableCalls);
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsOpenedAndMotorIsEnabled_expectMotorIsTurnedOn(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opened,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorOnCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(
		-nvmSettings.application.door.height,
		motorOnArgs[0],
		"Height");
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsOpenedAndMotorIsEnabled_expectMotorCurrentLimitIsNoLoad(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opened,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsMaximumLoadCalls, "M");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorLimitIsNoLoadCalls, "N");
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsOpenedAndMotorIsEnabled_expectMotorIsEnabledBeforeCurrentLimitIsChanged(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opened,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_TRUE(motorEnableSequence < motorLimitIsNoLoadSequence);
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsOpenedAndMotorIsEnabled_expectMotorCurrentLimitIsChangedBeforeMotorIsTurnedOn(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opened,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_TRUE(motorLimitIsNoLoadSequence < motorOnSequence);
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsOpenedAndMotorIsDisabled_expectClosingWaitingForEnabledMotorStateWithCloseTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opened,
		.transition = anyByteExcept(DoorTransition_Close)
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Closing_WaitingForEnabledMotor, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Close, state.transition, "T");
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsOpenedAndMotorIsDisabled_expectMotorIsEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opened,
		.transition = anyByte()
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorEnableCalls);
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsOpenedAndMotorIsDisabled_expectMotorIsNotTurnedOn(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opened,
		.transition = anyByte()
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsOpenedAndMotorIsDisabled_expectMotorCurrentLimitIsNotChanged(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opened,
		.transition = anyByte()
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsNoLoadCalls, "N");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsMaximumLoadCalls, "M");
}
