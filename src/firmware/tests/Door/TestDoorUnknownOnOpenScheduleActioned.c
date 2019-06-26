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

void onBeforeTest(void)
{
	doorFixtureInitialise();
}

void onAfterTest(void)
{
	doorFixtureShutdown();
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsUnknown_expectStateIsFindBottomWithOpenTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Unknown,
		.transition = anyByteExcept(DoorTransition_Open)
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_FindBottom, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsUnknownAndMotorIsEnabled_expectMotorIsEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Unknown,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorEnableCalls);
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsUnknownAndMotorIsDisabled_expectMotorIsEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Unknown,
		.transition = anyByte()
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorEnableCalls);
}
