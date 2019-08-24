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

void test_doorGetState_calledWhenInNonFaultState_expectNoFaultFlagsSet(void)
{
	struct DoorStateWithContext state =
	{
		.current = (enum DoorState) anyByteExcept(DoorState_Fault),
		.transition = (enum DoorTransition) anyByte(),
		.fault =
		{
			.all = anyByteExcept(0)
		}
	};

	stubDoorWithState(state.current, state.transition);
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, state.fault.all);
}

void test_doorGetState_calledWhenInFaultState_expectFaultFlagsSet(void)
{
	struct DoorStateWithContext state =
	{
		.current = (enum DoorState) anyByte(),
		.transition = (enum DoorTransition) anyByte(),
		.fault =
		{
			.all = 0
		}
	};

	uint8_t faults = anyByteExcept(0);
	stubDoorWithFault(faults);
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(faults, state.fault.all);
}

void test_doorGetState_calledWhenFaultStateCleared_expectNoFaultFlagsSet(void)
{
	struct DoorStateWithContext state =
	{
		.current = (enum DoorState) anyByteExcept(DoorState_Fault),
		.transition = (enum DoorTransition) anyByte(),
		.fault =
		{
			.all = anyByteExcept(0)
		}
	};

	uint8_t faults = anyByteExcept(0);
	stubDoorWithFault(faults);
	stubDoorWithState(state.current, state.transition);
	doorGetState(&state);

	TEST_ASSERT_EQUAL_UINT8(0, state.fault.all);
}
