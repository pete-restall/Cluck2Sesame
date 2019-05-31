#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Event.h"
#include "Motor.h"

#include "MotorFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

TEST_FILE("Motor/MotorInitialise.c")
TEST_FILE("Motor/MotorEnableDisable.c")
TEST_FILE("Motor/MotorOnOff.c")

void onBeforeTest(void)
{
	motorFixtureSetUp();
	motorInitialise();
}

void onAfterTest(void)
{
	motorFixtureTearDown();
}

void test_motorDisable_calledAfterEnable_expectVoltageRegulatorIsDisabled(void)
{
	motorEnable();
	motorDisable();
	TEST_ASSERT_EQUAL_UINT8(1, voltageRegulatorDisableCalls);
}

void test_motorDisable_calledBeforeEnable_expectVoltageRegulatorIsNotDisabled(void)
{
	motorDisable();
	TEST_ASSERT_EQUAL_UINT8(0, voltageRegulatorDisableCalls);
}

void test_motorDisable_calledMoreTimesThanEnable_expectVoltageRegulatorIsOnlyDisabledNumberOfTimesEnabled(void)
{
	motorEnable();
	motorDisable();
	motorDisable();
	TEST_ASSERT_EQUAL_UINT8(1, voltageRegulatorDisableCalls);
}

void test_motorDisable_called_expectMotorDisabledEventIsPublishedBeforeVoltageRegulatorIsDisabled(void)
{
	mockOnVoltageRegulatorDisabled();
	stubVoltageRegulatorDisableToPublishEvent();
	motorEnable();
	motorDisable();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorDisabledCalls, "MTR DIS calls");
	TEST_ASSERT_TRUE_MESSAGE(
		onMotorDisabledSequence < onVoltageRegulatorDisabledSequence,
		"Sequence");
}

void test_motorDisable_calledWhenStillEnabled_expectMotorDisabledEventIsNotPublished(void)
{
	motorEnable();
	motorEnable();
	motorDisable();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, onMotorDisabledCalls);
}

void test_motorDisable_calledMoreTimesThanEnabled_expectEventsDoNotGetOutOfSync(void)
{
	motorEnable();
	motorDisable();
	motorDisable();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, onMotorDisabledCalls);

	motorEnable();
	motorDisable();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(2, onMotorDisabledCalls);
}
