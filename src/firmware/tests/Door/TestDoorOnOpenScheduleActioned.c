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

void test_doorOpenScheduleActioned_onPublishedWhenActualStateIsFault_expectSameStateWithUnchangedTransition(void)
{
	struct DoorState state =
	{
		.actualState = DoorActualState_Fault,
		.transition = anyByteExcept(DoorTransition_Unchanged)
	};

	stubDoorWithState(state.actualState, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorActualState_Fault, state.actualState, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Unchanged, state.transition, "T");
}

void test_doorOpenScheduleActioned_onPublishedWhenActualStateIsOpened_expectSameStateWithUnchangedTransition(void)
{
	struct DoorState state =
	{
		.actualState = DoorActualState_Opened,
		.transition = anyByteExcept(DoorTransition_Unchanged)
	};

	stubDoorWithState(state.actualState, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorActualState_Opened, state.actualState, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Unchanged, state.transition, "T");
}

void test_doorOpenScheduleActioned_onPublishedWhenActualStateIsOpening_expectSameStateWithOpenTransition(void)
{
	struct DoorState state =
	{
		.actualState = DoorActualState_Opening,
		.transition = anyByteExcept(DoorTransition_Open)
	};

	stubDoorWithState(state.actualState, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorActualState_Opening, state.actualState, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}

void test_doorOpenScheduleActioned_onPublishedWhenActualStateIsClosed_expectSameStateWithOpenTransition(void)
{
	struct DoorState state =
	{
		.actualState = DoorActualState_Closed,
		.transition = anyByteExcept(DoorTransition_Open)
	};

	stubDoorWithState(state.actualState, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorActualState_Closed, state.actualState, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}

void test_doorOpenScheduleActioned_onPublishedWhenActualStateIsClosing_expectSameStateWithOpenTransition(void)
{
	struct DoorState state =
	{
		.actualState = DoorActualState_Closing,
		.transition = anyByteExcept(DoorTransition_Open)
	};

	stubDoorWithState(state.actualState, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorActualState_Closing, state.actualState, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}
