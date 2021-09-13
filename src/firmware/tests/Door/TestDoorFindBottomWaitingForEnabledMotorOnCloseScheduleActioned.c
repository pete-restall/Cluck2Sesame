#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/NvmSettings.h"
#include "ApplicationNvmSettings.h"
#include "Door.h"

#include "DoorFixture.h"
#include "DoorFindBottomFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"
#include "../NvmSettingsFixture.h"

TEST_FILE("Door/DoorInitialise.c")
TEST_FILE("Door/DoorGetState.c")
TEST_FILE("Door/DoorCalibrate.c")
TEST_FILE("Door/DoorOnAborted.c")
TEST_FILE("Door/DoorOnOpenScheduleActioned.c")
TEST_FILE("Door/DoorOnCloseScheduleActioned.c")
TEST_FILE("Door/DoorOnMotorStopped.c")
TEST_FILE("Door/DoorOnMotorEnabled.c")

void test_doorCloseScheduleActioned_onPublishedWhenStateIsFindBottomWaitingForEnabledMotor_expectMotorIsNotEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_FindBottom_WaitingForEnabledMotor,
		.transition = anyByte()
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, motorEnableCalls);
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsFindBottomWaitingForEnabledMotor_expectMotorIsNotTurnedOn(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_FindBottom_WaitingForEnabledMotor,
		.transition = anyByte()
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, motorOnCalls);
}

void test_doorCloseScheduleActioned_onPublishedWhenStateIsFindBottomWaitingForEnabledMotor_expectMotorCurrentLimitIsNotChanged(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_FindBottom_WaitingForEnabledMotor,
		.transition = anyByte()
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsNoLoadCalls, "N");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsMaximumLoadCalls, "M");
}
