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

void test_motorOn_calledWithPositiveCount_expectCcpLimitIsSameValue(void)
{
	ensureMotorFullyEnabled();
	int16_t clockwise = anyClockwiseCount();
	motorOn(clockwise);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE((clockwise >> 8) & 0xff, CCPR1H, "CCPR1H");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE((clockwise >> 0) & 0xff, CCPR1L, "CCPR1L");
}

void test_motorOn_calledWithNegativeCount_expectCcpLimitIsNegatedValue(void)
{
	ensureMotorFullyEnabled();
	int16_t antiClockwise = anyAntiClockwiseCount();
	motorOn(antiClockwise);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE((-antiClockwise >> 8) & 0xff, CCPR1H, "CCPR1H");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE((-antiClockwise >> 0) & 0xff, CCPR1L, "CCPR1L");
}

void test_motorOn_calledWithMaximumNegativeCount_expectCcpLimitIsMaximumPositiveValue(void)
{
	ensureMotorFullyEnabled();
	motorOn(-32768);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x7f, CCPR1H, "CCPR1H");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0xff, CCPR1L, "CCPR1L");
}

void test_motorOn_called_expectTimer1IsCleared(void)
{
	ensureMotorFullyEnabled();
	TMR1H = anyByteExcept(0);
	TMR1L = anyByteExcept(0);
	motorOn(anyEncoderCount());
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, TMR1L, "TMR1L");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, TMR1H, "TMR1H");
}

void test_motorOn_called_expectTimer1InterruptFlagIsCleared(void)
{
	ensureMotorFullyEnabled();
	PIR4 = anyByteWithMaskSet(_PIR4_TMR1IF_MASK);
	uint8_t originalPir4 = PIR4;
	motorOn(anyEncoderCount());
	TEST_ASSERT_EQUAL_UINT8(originalPir4 & ~_PIR4_TMR1IF_MASK, PIR4);
}

void test_motorOn_calledWithZero_expectCcpLimitIsNotCleared(void)
{
	ensureMotorFullyEnabled();
	CCPR1H = anyByteExcept(0);
	CCPR1L = anyByteExcept(0);
	uint8_t originalCcpr1h = CCPR1H;
	uint8_t originalCcpr1l = CCPR1L;
	motorOn(0);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalCcpr1h, CCPR1H, "CCPR1H");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalCcpr1l, CCPR1L, "CCPR1L");
}

void test_motorOn_calledWithZero_expectTimer1IsNotCleared(void)
{
	ensureMotorFullyEnabled();
	TMR1H = anyByteExcept(0);
	TMR1L = anyByteExcept(0);
	uint8_t originalTmr1l = TMR1L;
	uint8_t originalTmr1h = TMR1H;
	motorOn(0);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalTmr1l, TMR1L, "TMR1L");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalTmr1h, TMR1H, "TMR1H");
}

void test_motorOn_calledWithZero_expectTimer1InterruptFlagIsNotCleared(void)
{
	ensureMotorFullyEnabled();
	PIR4bits.TMR1IF = 1;
	motorOn(0);
	TEST_ASSERT_TRUE(PIR4bits.TMR1IF);
}

void test_motorOn_called_expectCcpModeIsCompareWithSetAndHoldOutputWithTmr1Preserved(void)
{
	ensureMotorFullyEnabled();
	CCP1CON = anyByteWithMaskSet(_CCP1CON_MODE1_MASK);
	uint8_t originalCcp1conWithClearMode =
		(CCP1CON & ~(0b1111 << _CCP1CON_MODE_POSITION)) | _CCP1CON_OUT_MASK;

	motorOn(anyEncoderCount());
	TEST_ASSERT_EQUAL_UINT8(
		originalCcp1conWithClearMode | (0b1000 << _CCP1CON_MODE_POSITION),
		CCP1CON | _CCP1CON_OUT_MASK);
}

void test_motorOn_calledWithZero_expectCcpModeIsUnchanged(void)
{
	ensureMotorFullyEnabled();
	CCP1CON = anyByteWithMaskSet(_CCP1CON_MODE1_MASK);
	uint8_t originalCcp1con = CCP1CON | _CCP1CON_OUT_MASK;

	motorOn(0);
	TEST_ASSERT_EQUAL_UINT8(originalCcp1con, CCP1CON | _CCP1CON_OUT_MASK);
}

void test_motorOn_called_expectShutdownFlagIsCleared(void)
{
	ensureMotorFullyEnabled();
	CWG1AS0 = anyByteWithMaskSet(_CWG1AS0_SHUTDOWN_MASK);
	uint8_t originalCwg1as0 = CWG1AS0;
	motorOn(anyEncoderCount());
	TEST_ASSERT_EQUAL_UINT8(originalCwg1as0 & ~_CWG1AS0_SHUTDOWN_MASK, CWG1AS0);
}

void test_motorOn_calledWithZero_expectShutdownFlagIsNotCleared(void)
{
	ensureMotorFullyEnabled();
	CWG1AS0 = anyByteWithMaskSet(_CWG1AS0_SHUTDOWN_MASK);
	uint8_t originalCwg1as0 = CWG1AS0;
	motorOn(0);
	TEST_ASSERT_EQUAL_UINT8(originalCwg1as0, CWG1AS0);
}

void test_motorOn_calledWithClockwiseCountWhenNotTurning_expectPwmSteeringToStrb(void)
{
	ensureMotorFullyEnabled();
	CWG1STR = anyByteWithMaskClear(STEERING_MASK);
	uint8_t originalCwg1strWithClearSteering = CWG1STR;
	motorOn(anyClockwiseCount());
	TEST_ASSERT_EQUAL_UINT8(
		originalCwg1strWithClearSteering | _CWG1STR_STRB_MASK,
		CWG1STR);
}

void test_motorOn_calledWithClockwiseCountWhenTurning_expectPwmSteeringIsNotModified(void)
{
	ensureMotorFullyEnabled();
	CWG1STR = anyByteWithMaskSet(STEERING_MASK) & ~_CWG1STR_STRB_MASK;
	uint8_t originalCwg1str = CWG1STR;
	motorOn(anyClockwiseCount());
	TEST_ASSERT_EQUAL_UINT8(originalCwg1str, CWG1STR);
}

void test_motorOn_calledWithAntiClockwiseCountWhenNotTurning_expectPwmSteeringToStra(void)
{
	ensureMotorFullyEnabled();
	CWG1STR = anyByteWithMaskClear(STEERING_MASK);
	uint8_t originalCwg1strWithClearSteering = CWG1STR;
	motorOn(anyAntiClockwiseCount());
	TEST_ASSERT_EQUAL_UINT8(
		originalCwg1strWithClearSteering | _CWG1STR_STRA_MASK,
		CWG1STR);
}

void test_motorOn_calledWithAntiClockwiseCountWhenTurning_expectPwmSteeringIsNotModified(void)
{
	ensureMotorFullyEnabled();
	CWG1STR = anyByteWithMaskSet(STEERING_MASK) & ~_CWG1STR_STRA_MASK;
	uint8_t originalCwg1str = CWG1STR;
	motorOn(anyAntiClockwiseCount());
	TEST_ASSERT_EQUAL_UINT8(originalCwg1str, CWG1STR);
}

void test_motorOn_calledWithZeroWhenTurning_expectPwmSteeringIsNotModified(void)
{
	ensureMotorFullyEnabled();
	CWG1STR = anyByteWithMaskSet(STEERING_MASK) & ~_CWG1STR_STRA_MASK;
	uint8_t originalCwg1str = CWG1STR;
	motorOn(0);
	TEST_ASSERT_EQUAL_UINT8(originalCwg1str, CWG1STR);
}

// TODO: motorOn() - probably shouldn't clear TMR1IF; that should be the 'on woken from sleep' handler's responsibility (also make sure TMR1IE is set in motorInitialise())
