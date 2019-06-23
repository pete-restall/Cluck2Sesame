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

void onBeforeTest(void)
{
	doorFixtureInitialise();
}

void onAfterTest(void)
{
	doorFixtureShutdown();
}

void test_doorGetState_calledAfterInitialised_expectActualStateIsUnknown(void)
{
	struct DoorState state =
	{
		.actualState = anyByteExcept(DoorActualState_Unknown)
	};
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(DoorActualState_Unknown, state.actualState);
}

void test_doorGetState_calledAfterInitialised_expectTargetStateIsUnchanged(void)
{
	struct DoorState state =
	{
		.targetState = anyByteExcept(DoorTargetState_Unchanged)
	};
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(DoorTargetState_Unchanged, state.targetState);
}

void test_doorGetState_calledWhenDoorIsTimeDriven_expectCorrectModeFlags(void)
{
	stubNvmSettingsForTimeDrivenMode();
	doorInitialise();

	struct DoorState state = { .flags = anyByte() };
	doorGetState(&state);

	TEST_ASSERT_TRUE_MESSAGE(state.flags.isTimeDriven, "T");
	TEST_ASSERT_FALSE_MESSAGE(state.flags.isSunEventDriven, "S");
	TEST_ASSERT_FALSE_MESSAGE(state.flags.isManuallyOverridden, "M");
}

void test_doorGetState_calledWhenDoorIsSunEventDriven_expectCorrectModeFlags(void)
{
	stubNvmSettingsForSunEventDrivenMode();
	doorInitialise();

	struct DoorState state = { .flags = anyByte() };
	doorGetState(&state);

	TEST_ASSERT_FALSE_MESSAGE(state.flags.isTimeDriven, "T");
	TEST_ASSERT_TRUE_MESSAGE(state.flags.isSunEventDriven, "S");
	TEST_ASSERT_FALSE_MESSAGE(state.flags.isManuallyOverridden, "M");
}

void test_doorGetState_calledWhenDoorIsManuallyOverridden_expectCorrectModeFlags(void)
{
	stubNvmSettingsForManuallyDrivenMode();
	doorInitialise();

	struct DoorState state = { .flags = anyByte() };
	doorGetState(&state);

	TEST_ASSERT_FALSE_MESSAGE(state.flags.isTimeDriven, "T");
	TEST_ASSERT_FALSE_MESSAGE(state.flags.isSunEventDriven, "S");
	TEST_ASSERT_TRUE_MESSAGE(state.flags.isManuallyOverridden, "M");
}

void test_doorGetState_calledWhenDoorIsInUnspecifiedMode_expectCorrectModeFlags(void)
{
	stubNvmSettingsForUnspecifiedMode();
	doorInitialise();

	struct DoorState state = { .flags = anyByte() };
	doorGetState(&state);

	TEST_ASSERT_FALSE_MESSAGE(state.flags.isTimeDriven, "T");
	TEST_ASSERT_FALSE_MESSAGE(state.flags.isSunEventDriven, "S");
	TEST_ASSERT_TRUE_MESSAGE(state.flags.isManuallyOverridden, "M");
}
