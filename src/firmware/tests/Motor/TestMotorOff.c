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

void test_motorOff_called_expectCcpLimitIsSameValue(void)
{
	ensureMotorFullyEnabled();
	CWG1STR = anyByteWithMaskClear(STEERING_MASK);
	uint8_t originalCwg1strWithClearSteering = CWG1STR;
	motorOn(anyEncoderCount());

	motorOff();
	TEST_ASSERT_EQUAL_UINT8(originalCwg1strWithClearSteering, CWG1STR);
}

// TODO: motorOff() - CWG1STRbits.STRA = 0, CWG1STRbits.STRB = 0
// TODO: motorOff() - TMR1 not cleared, CCPR1 not cleared
// TODO: motorOff() - MOTOR_STOPPED EVENT TESTS
