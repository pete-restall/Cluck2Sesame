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

#define STEERING_MASK ( \
		_CWG1STR_STRA_MASK | \
		_CWG1STR_STRB_MASK | \
		_CWG1STR_STRC_MASK | \
		_CWG1STR_STRD_MASK)

void onBeforeTest(void)
{
	motorFixtureSetUp();
	motorInitialise();
}

void onAfterTest(void)
{
	motorFixtureTearDown();
}

void test_motorOn_calledWithClockwiseCount_expectPwmDutyCycleIsZero(void)
{
	ensureMotorFullyEnabled();
	PWM4DCH = anyByteExcept(0);
	PWM4DCL = anyByteExcept(0);
	uint8_t originalPwm4dcl = PWM4DCL;
	uint8_t originalPwm4dch = PWM4DCH;
	motorOn(anyClockwiseCount());
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, PWM4DCL, "PWM4DCL");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, PWM4DCH, "PWM4DCH");
}

void test_motorOn_calledWithAntiClockwiseCount_expectPwmDutyCycleIsZero(void)
{
	ensureMotorFullyEnabled();
	PWM4DCH = anyByteExcept(0);
	PWM4DCL = anyByteExcept(0);
	uint8_t originalPwm4dcl = PWM4DCL;
	uint8_t originalPwm4dch = PWM4DCH;
	motorOn(anyAntiClockwiseCount());
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, PWM4DCL, "PWM4DCL");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, PWM4DCH, "PWM4DCH");
}

// TODO: motorOn() - turning, the PWM duty cycle should not be modified
// TODO: motorOn() - start off the PWM soft-start (nearScheduleAdd(...))
// TODO: motorOn() - MOTOR_STARTED event tests
