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

void test_doorGetState_calledAfterInitialised_expectStateIsUnknown(void)
{
	struct DoorStateWithContext state =
	{
		.current = anyByteExcept(DoorState_Unknown)
	};
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(DoorState_Unknown, state.current);
}

void test_doorGetState_calledAfterInitialised_expectTransitionIsUnchanged(void)
{
	struct DoorStateWithContext state =
	{
		.transition = anyByteExcept(DoorTransition_Unchanged)
	};
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(DoorTransition_Unchanged, state.transition);
}

void test_doorGetState_calledWhenDoorIsTimeDriven_expectCorrectModeFlags(void)
{
	stubNvmSettingsForTimeDrivenMode();
	doorInitialise();

	struct DoorStateWithContext state = { .flags = anyByte() };
	doorGetState(&state);

	TEST_ASSERT_TRUE_MESSAGE(state.flags.isTimeDriven, "T");
	TEST_ASSERT_FALSE_MESSAGE(state.flags.isSunEventDriven, "S");
	TEST_ASSERT_FALSE_MESSAGE(state.flags.isManuallyOverridden, "M");
}

void test_doorGetState_calledWhenDoorIsSunEventDriven_expectCorrectModeFlags(void)
{
	stubNvmSettingsForSunEventDrivenMode();
	doorInitialise();

	struct DoorStateWithContext state = { .flags = anyByte() };
	doorGetState(&state);

	TEST_ASSERT_FALSE_MESSAGE(state.flags.isTimeDriven, "T");
	TEST_ASSERT_TRUE_MESSAGE(state.flags.isSunEventDriven, "S");
	TEST_ASSERT_FALSE_MESSAGE(state.flags.isManuallyOverridden, "M");
}

void test_doorGetState_calledWhenDoorIsManuallyOverridden_expectCorrectModeFlags(void)
{
	stubNvmSettingsForManuallyDrivenMode();
	doorInitialise();

	struct DoorStateWithContext state = { .flags = anyByte() };
	doorGetState(&state);

	TEST_ASSERT_FALSE_MESSAGE(state.flags.isTimeDriven, "T");
	TEST_ASSERT_FALSE_MESSAGE(state.flags.isSunEventDriven, "S");
	TEST_ASSERT_TRUE_MESSAGE(state.flags.isManuallyOverridden, "M");
}

void test_doorGetState_calledWhenDoorIsInUnspecifiedMode_expectCorrectModeFlags(void)
{
	stubNvmSettingsForUnspecifiedMode();
	doorInitialise();

	struct DoorStateWithContext state = { .flags = anyByte() };
	doorGetState(&state);

	TEST_ASSERT_FALSE_MESSAGE(state.flags.isTimeDriven, "T");
	TEST_ASSERT_FALSE_MESSAGE(state.flags.isSunEventDriven, "S");
	TEST_ASSERT_TRUE_MESSAGE(state.flags.isManuallyOverridden, "M");
}
