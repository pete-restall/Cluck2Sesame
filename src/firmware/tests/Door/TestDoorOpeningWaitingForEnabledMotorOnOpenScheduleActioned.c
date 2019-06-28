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

void test_doorOpenScheduleActioned_onPublishedWhenStateIsOpeningWaitingForEnabledMotor_expectSameStateWithOpenTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opening_WaitingForEnabledMotor,
		.transition = anyByteExcept(DoorTransition_Open)
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Opening_WaitingForEnabledMotor, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsOpeningWaitingForEnabledMotor_expectMotorIsNotEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opening_WaitingForEnabledMotor,
		.transition = anyByte()
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, motorEnableCalls);
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsOpeningWaitingForEnabledMotor_expectMotorIsNotTurnedOn(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opening_WaitingForEnabledMotor,
		.transition = anyByte()
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsOpeningWaitingForEnabledMotor_expectMotorCurrentLimitIsNotChanged(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opening_WaitingForEnabledMotor,
		.transition = anyByte()
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsNoLoadCalls, "N");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsMaximumLoadCalls, "M");
}
