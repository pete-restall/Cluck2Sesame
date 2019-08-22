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

void test_motorStopped_onPublishedWithFaults_expectDoorOpenedIsNotPublished(void)
{
	uint8_t anyTransition = anyByte();
	stubDoorWithState(DoorState_Opening, anyTransition);
	publishMotorStoppedWithFaults();
	mockOnDoorOpened();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, onDoorOpenedCalls, "Calls");
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsOpeningAndTransitionIsOpen_expectMotorIsDisabled(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Open);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsOpeningAndTransitionIsUnchanged_expectMotorIsDisabled(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Unchanged);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsOpeningAndTransitionIsClose_expectMotorIsNotDisabled(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Close);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, motorDisableCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsOpeningAndTransitionIsOpen_expectMotorIsDisabled(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Open);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsOpeningAndTransitionIsUnchanged_expectMotorIsDisabled(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Unchanged);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsOpeningAndTransitionIsClose_expectMotorIsDisabled(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Close);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, motorDisableCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsOpeningAndTransitionIsOpen_expectMotorCurrentLimitIsUnchanged(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Open);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsNoLoadCalls);
	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsMaximumLoadCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsOpeningAndTransitionIsUnchanged_expectMotorCurrentLimitIsUnchanged(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Unchanged);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsNoLoadCalls);
	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsMaximumLoadCalls);
}

void test_motorStopped_onPublishedWithNoFaultsWhenStateIsOpeningAndTransitionIsClose_expectMotorCurrentLimitIsNoLoad(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Close);
	publishMotorStoppedWithNoFaults();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8(1, motorLimitIsNoLoadCalls);
	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsMaximumLoadCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsOpeningAndTransitionIsOpen_expectMotorCurrentLimitIsUnchanged(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Open);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsNoLoadCalls);
	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsMaximumLoadCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsOpeningAndTransitionIsUnchanged_expectMotorCurrentLimitIsUnchanged(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Unchanged);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsNoLoadCalls);
	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsMaximumLoadCalls);
}

void test_motorStopped_onPublishedWithFaultsWhenStateIsOpeningAndTransitionIsClose_expectMotorCurrentLimitIsUnchanged(void)
{
	stubMotorIsEnabled();
	stubDoorWithState(DoorState_Opening, DoorTransition_Close);
	publishMotorStoppedWithFaults();
	dispatchAllEvents();

	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsNoLoadCalls);
	TEST_ASSERT_EQUAL_UINT8(0, motorLimitIsMaximumLoadCalls);
}
