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
	motorOn(anyClockwiseCount());
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, PWM4DCL, "PWM4DCL");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, PWM4DCH, "PWM4DCH");
}

void test_motorOn_calledWithAntiClockwiseCount_expectPwmDutyCycleIsZero(void)
{
	ensureMotorFullyEnabled();
	PWM4DCH = anyByteExcept(0);
	PWM4DCL = anyByteExcept(0);
	motorOn(anyAntiClockwiseCount());
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, PWM4DCL, "PWM4DCL");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, PWM4DCH, "PWM4DCH");
}

void test_motorOn_calledWithZeroCount_expectPwmDutyCycleIsNotModified(void)
{
	ensureMotorFullyEnabled();
	PWM4DCH = anyByteExcept(0);
	PWM4DCL = anyByteExcept(0);
	uint8_t originalPwm4dcl = PWM4DCL;
	uint8_t originalPwm4dch = PWM4DCH;
	motorOn(0);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalPwm4dcl, PWM4DCL, "PWM4DCL");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalPwm4dch, PWM4DCH, "PWM4DCH");
}

void test_motorOn_calledWhenMotorTurning_expectPwmDutyCycleIsNotModified(void)
{
	ensureMotorFullyEnabled();
	PWM4DCH = anyByteExcept(0);
	PWM4DCL = anyByteExcept(0);
	uint8_t originalPwm4dcl = PWM4DCL;
	uint8_t originalPwm4dch = PWM4DCH;

	CWG1STR = anyByte();
	if (!(CWG1STR & STEERING_MASK))
		CWG1STR |= _CWG1STR_STRC_MASK;

	motorOn(anyEncoderCount());
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalPwm4dcl, PWM4DCL, "PWM4DCL");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalPwm4dch, PWM4DCH, "PWM4DCH");
}

void test_motorOn_calledWithZeroCount_expectMotorStartedEventIsNotPublished(void)
{
	ensureMotorFullyEnabled();
	motorOn(0);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, onMotorStartedCalls);
}

void test_motorOn_called_expectMotorStartedEventIsPublishedWithSameCount(void)
{
	ensureMotorFullyEnabled();
	int16_t count = anyEncoderCount();
	motorOn(count);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStartedCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(count, onMotorStartedArgs.count, "Count");
}

// TODO: motorOn() - start off the PWM soft-start (nearScheduleAdd(...))
