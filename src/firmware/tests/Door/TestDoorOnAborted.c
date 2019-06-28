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

void test_doorAborted_onPublished_expectStateIsFaultAndTargetIsUnchanged(void)
{
	struct DoorStateWithContext state =
	{
		.current = anyByteExcept(DoorState_Fault),
		.transition = anyByteExcept(DoorTransition_Unchanged)
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorAbortedWithAnyFault();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Fault, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Unchanged, state.transition, "T");
}
