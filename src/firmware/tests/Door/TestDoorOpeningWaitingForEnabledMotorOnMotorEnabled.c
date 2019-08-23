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

void test_motorEnabled_onPublishedWhenStateIsOpeningWaitingForEnabledMotor_expectOpeningStateWithUnchangedTransition(void)
{
	uint8_t anyTransition = anyByteExcept(DoorTransition_Open);
	stubDoorWithState(DoorState_Opening_WaitingForEnabledMotor, anyTransition);
	stubMotorIsEnabled();
	publishMotorEnabled();
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Opening, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(anyTransition, state.transition, "T");
}

void test_motorEnabled_onPublishedWhenStateIsOpeningWaitingForEnabledMotorAndMotorIsDisabled_expectUnknownStateWithUnchangedTransition(void)
{
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_Opening_WaitingForEnabledMotor, anyTransition);
	stubMotorIsDisabled();
	publishMotorEnabled();
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Unknown, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(anyTransition, state.transition, "T");
}

void test_motorEnabled_onPublishedWhenStateIsOpeningWaitingForEnabledMotor_expectMotorCurrentLimitIsMaximumLoad(void)
{
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_Opening_WaitingForEnabledMotor, anyTransition);
	stubMotorIsEnabled();
	publishMotorEnabled();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsNoLoadCalls, "N");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorLimitIsMaximumLoadCalls, "M");
}

void test_motorEnabled_onPublishedWhenStateIsOpeningWaitingForEnabledMotorAndMotorIsDisabled_expectMotorCurrentLimitIsNotModified(void)
{
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_Opening_WaitingForEnabledMotor, anyTransition);
	stubMotorIsDisabled();
	publishMotorEnabled();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsNoLoadCalls, "N");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorLimitIsMaximumLoadCalls, "M");
}

void test_motorEnabled_onPublishedWhenStateIsOpeningWaitingForEnabledMotor_expectMotorIsTurnedOnAfterCurrentLimitIsSet(void)
{
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_Opening_WaitingForEnabledMotor, anyTransition);
	stubMotorIsEnabled();
	publishMotorEnabled();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorOnCalls, "Calls");
	TEST_ASSERT_TRUE_MESSAGE(motorOnSequence > motorLimitIsMaximumLoadSequence, "Seq");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(-nvmSettings.application.door.height, motorOnArgs[0], "Arg");
}

void test_motorEnabled_onPublishedWhenStateIsOpeningWaitingForEnabledMotorAndMotorIsDisabled_expectMotorIsNotTurnedOn(void)
{
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_Opening_WaitingForEnabledMotor, anyTransition);
	stubMotorIsDisabled();
	publishMotorEnabled();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, motorOnCalls, "Calls");
}
