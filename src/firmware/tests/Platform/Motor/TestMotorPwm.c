#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/Motor.h"

#include "MotorFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Motor/MotorInitialise.c")
TEST_FILE("Platform/Motor/MotorEnableDisable.c")
TEST_FILE("Platform/Motor/MotorOnOff.c")

void onBeforeTest(void)
{
	motorFixtureSetUp();
	motorInitialise();
	PMD3bits.PWM4MD = 0;
}

void onAfterTest(void)
{
	motorFixtureTearDown();
}

void test_voltageRegulatorEnabled_onPublished_expectPwmIsEnabled(void)
{
	PWM4CON = anyByteWithMaskClear(_PWM4CON_PWM4EN_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_TRUE(PWM4CONbits.PWM4EN);
}

void test_voltageRegulatorEnabled_onPublished_expectPwmOutputIsNotInverted(void)
{
	PWM4CON = anyByteWithMaskSet(_PWM4CON_PWM4POL_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_FALSE(PWM4CONbits.PWM4POL);
}

void test_voltageRegulatorEnabled_onPublished_expectPwmDutyCycleIsZero(void)
{
	PWM4DCH = anyByteExcept(0);
	PWM4DCL = anyByteExcept(0);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, PWM4DCH, "PWM4DCH");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, PWM4DCL, "PWM4DCL");
}
