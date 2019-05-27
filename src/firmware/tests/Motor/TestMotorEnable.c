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

void onBeforeTest(void)
{
	motorFixtureSetUp();
	motorInitialise();
}

void onAfterTest(void)
{
	motorFixtureTearDown();
}

void test_motorEnable_calledWhenVoltageRegulatorIsDisabled_expectVoltageRegulatorIsEnabled(void)
{
	stubVoltageRegulatorIsEnabled(0);
	motorEnable();
	TEST_ASSERT_EQUAL_UINT8(1, voltageRegulatorEnableCalls);
}

void test_motorEnable_calledWhenVoltageRegulatorIsEnabled_expectVoltageRegulatorIsEnabledAgain(void)
{
	stubVoltageRegulatorIsEnabled(1);
	motorEnable();
	TEST_ASSERT_EQUAL_UINT8(1, voltageRegulatorEnableCalls);
}

void test_motorEnable_calledMoreThanOnce_expectVoltageRegulatorIsEnabledSameNumberOfTimes(void)
{
	uint8_t numberOfCalls = anyByteLessThan(10);
	for (uint8_t i = 0; i < numberOfCalls; i++)
		motorEnable();

	TEST_ASSERT_EQUAL_UINT8(numberOfCalls, voltageRegulatorEnableCalls);
}

void test_motorEnable_calledForFirstTimeWhenVoltageRegulatorIsAlreadyEnabled_expectMotorEnabledEventIsPublished(void)
{
	stubVoltageRegulatorIsEnabled(1);
	motorEnable();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, onMotorEnabledCalls);
}

void test_motorEnable_calledForFirstTimeWhenVoltageRegulatorIsNotEnabled_expectMotorEnabledEventIsNotPublished(void)
{
	stubVoltageRegulatorIsEnabled(0);
	motorEnable();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, onMotorEnabledCalls);
}

void test_motorEnable_calledForSecondTimeWhenVoltageRegulatorIsAlreadyEnabled_expectMotorEnabledEventIsNotPublished(void)
{
	stubVoltageRegulatorIsEnabled(1);
	motorEnable();
	dispatchAllEvents();
	motorEnable();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, onMotorEnabledCalls);
}

void test_motorEnable_calledForSecondTimeWhenVoltageRegulatorIsNotEnabled_expectMotorEnabledEventIsNotPublished(void)
{
	stubVoltageRegulatorIsEnabled(0);
	motorEnable();
	motorEnable();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, onMotorEnabledCalls);
}

void test_motorEnable_calledForFirstTimeWhenVoltageRegulatorIsNotEnabled_expectMotorEnabledEventIsPublishedWhenVoltageRegulatorIsEnabled(void)
{
	stubVoltageRegulatorIsEnabled(0);
	motorEnable();
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, onMotorEnabledCalls);
}

void test_motorEnable_calledTwiceWhenVoltageRegulatorIsNotEnabled_expectOneMotorEnabledEventIsPublishedWhenVoltageRegulatorIsEnabled(void)
{
	stubVoltageRegulatorIsEnabled(0);
	motorEnable();
	motorEnable();
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(1, onMotorEnabledCalls);
}

void test_voltageRegulatorEnabled_onPublishedBeforeFirstMotorEnabledCall_expectMotorEnabledEventIsNotPublished(void)
{
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, onMotorEnabledCalls);
}

void test_motorIsEnabled_calledBeforeEnabled_expectFalse(void)
{
	TEST_ASSERT_FALSE(motorIsEnabled());
}

void test_motorIsEnabled_calledBeforeMotorEnabledEvent_expectFalse(void)
{
	stubVoltageRegulatorIsEnabled(0);
	motorEnable();
	TEST_ASSERT_FALSE(motorIsEnabled());
}

void test_motorIsEnabled_calledAfterMotorEnabledEvent_expectTrue(void)
{
	stubVoltageRegulatorIsEnabled(0);
	motorEnable();
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_TRUE(motorIsEnabled());
}

void test_motorIsEnabled_calledWhenVoltageRegulatorEnabledButMotorEnabledEventHasNotBeenPublished_expectFalse(void)
{
	stubVoltageRegulatorIsEnabled(1);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_FALSE(motorIsEnabled());
}

void test_motorIsEnabled_calledWhenVoltageRegulatorEnabledAndMotorEnableHasBeenCalled_expectTrue(void)
{
	stubVoltageRegulatorIsEnabled(1);
	motorEnable();
	TEST_ASSERT_TRUE(motorIsEnabled());
}

void test_motorIsEnabled_calledAfterDisabled_expectFalse(void)
{
	stubVoltageRegulatorIsEnabled(1);
	motorEnable();
	dispatchAllEvents();
	motorDisable();
	TEST_ASSERT_FALSE(motorIsEnabled());
}

void test_motorIsEnabled_calledAfterDisableButStillEnabled_expectTrue(void)
{
	stubVoltageRegulatorIsEnabled(1);
	motorEnable();
	motorEnable();
	dispatchAllEvents();
	motorDisable();
	TEST_ASSERT_TRUE(motorIsEnabled());
}
