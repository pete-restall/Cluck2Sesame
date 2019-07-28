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

void test_doorCloseScheduleActioned_onPublishedWhenStateIsUnknownAndMotorIsEnabled_expectStateIsFindBottomWithCloseTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Unknown,
		.transition = anyByteExcept(DoorTransition_Close)
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_FindBottom, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Close, state.transition, "T");
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsUnknownAndMotorIsEnabled_expectMotorIsEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Unknown,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8(1, motorEnableCalls);
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsUnknownAndMotorIsEnabled_expectMotorIsLoweredAbout10cm(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Unknown,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorOnCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(-PULSES_PER_10CM, motorOnArgs[0], "Arg");
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsUnknownAndMotorIsEnabled_expectMotorCurrentLimitIsNoLoad(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Unknown,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsMaximumLoadCalls, "M");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorLimitIsNoLoadCalls, "N");
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsUnknownAndMotorIsEnabled_expectMotorIsEnabledBeforeCurrentLimitIsChanged(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Unknown,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();

	TEST_ASSERT_TRUE(motorEnableSequence < motorLimitIsNoLoadSequence);
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsUnknownAndMotorIsEnabled_expectMotorCurrentLimitIsChangedBeforeMotorIsTurnedOn(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Unknown,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();

	TEST_ASSERT_TRUE(motorLimitIsNoLoadSequence < motorOnSequence);
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsUnknownAndMotorIsDisabled_expectStateIsFindBottomWaitingForEnabledMotorWithCloseTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Unknown,
		.transition = anyByteExcept(DoorTransition_Close)
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_FindBottom_WaitingForEnabledMotor, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Close, state.transition, "T");
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsUnknownAndMotorIsDisabled_expectMotorIsEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Unknown,
		.transition = anyByte()
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8(1, motorEnableCalls);
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsUnknownAndMotorIsDisabled_expectMotorIsNotTurnedOn(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Unknown,
		.transition = anyByte()
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsUnknownAndMotorIsDisabled_expectMotorCurrentLimitIsNotChanged(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Unknown,
		.transition = anyByte()
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsNoLoadCalls, "N");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsMaximumLoadCalls, "M");
}
