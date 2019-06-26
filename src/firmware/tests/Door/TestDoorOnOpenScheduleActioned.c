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

void test_doorOpenScheduleActioned_onPublishedWhenStateIsFault_expectSameStateWithOpenTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Fault,
		.transition = anyByteExcept(DoorTransition_Open)
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Fault, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsFault_expectMotorIsNotEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Fault,
		.transition = anyByte()
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, motorEnableCalls);
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsOpened_expectSameStateWithUnchangedTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opened,
		.transition = anyByteExcept(DoorTransition_Unchanged)
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Opened, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Unchanged, state.transition, "T");
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsOpened_expectMotorIsNotEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opened,
		.transition = anyByte()
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, motorEnableCalls);
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsOpening_expectSameStateWithOpenTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opening,
		.transition = anyByteExcept(DoorTransition_Open)
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Opening, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsOpening_expectMotorIsNotEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Opening,
		.transition = anyByte()
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, motorEnableCalls);
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

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosedAndMotorIsEnabled_expectOpeningStateWithOpenTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closed,
		.transition = anyByteExcept(DoorTransition_Open)
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Opening, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosedAndMotorIsEnabled_expectMotorIsEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closed,
		.transition = anyByte()
	};

	stubMotorIsEnabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorEnableCalls);
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosedAndMotorIsDisabled_expectOpeningWaitingForEnabledMotorStateWithOpenTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closed,
		.transition = anyByteExcept(DoorTransition_Open)
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Opening_WaitingForEnabledMotor, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosedAndMotorIsDisabled_expectMotorIsEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closed,
		.transition = anyByte()
	};

	stubMotorIsDisabled();
	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(1, motorEnableCalls);
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosing_expectSameStateWithOpenTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closing,
		.transition = anyByteExcept(DoorTransition_Open)
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_Closing, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsClosing_expectMotorIsNotEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_Closing,
		.transition = anyByte()
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, motorEnableCalls);
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsFindBottom_expectSameStateWithOpenTransition(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_FindBottom,
		.transition = anyByteExcept(DoorTransition_Open)
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorState_FindBottom, state.current, "A");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(DoorTransition_Open, state.transition, "T");
}

void test_doorOpenScheduleActioned_onPublishedWhenStateIsFindBottom_expectMotorIsNotEnabled(void)
{
	struct DoorStateWithContext state =
	{
		.current = DoorState_FindBottom,
		.transition = anyByte()
	};

	stubDoorWithState(state.current, state.transition);
	publishDoorOpenScheduleActioned();
	dispatchAllEvents();
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, motorEnableCalls);
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
