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

#define PULSES_PER_10CM 1576

static void enterFindBottomState(void);

void onBeforeTest(void)
{
	doorFixtureInitialise();
}

void onAfterTest(void)
{
	doorFixtureShutdown();
}

void test_findBottom_expectDoorIsFirstLoweredAbout10cm(void)
{
	enterFindBottomState();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, motorOnCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(motorOnArgs[0], -PULSES_PER_10CM, "Arg");
}

static void enterFindBottomState(void)
{
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_Unknown, anyTransition);
	stubMotorIsEnabled();
	publishDoorCloseScheduleActioned();
	dispatchAllEvents();

	struct DoorStateWithContext state;
	doorGetState(&state);
	TEST_ASSERT_EQUAL_UINT8(DoorState_FindBottom, state.current);
}
