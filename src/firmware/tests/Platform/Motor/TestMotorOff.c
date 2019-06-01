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
}

void onAfterTest(void)
{
	motorFixtureTearDown();
}

void test_motorOff_called_expectCcpLimitIsSameValue(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	uint8_t originalCcpr1h = CCPR1H;
	uint8_t originalCcpr1l = CCPR1L;

	motorOff();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalCcpr1h, CCPR1H, "CCPR1H");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalCcpr1l, CCPR1L, "CCPR1L");
}

void test_motorOff_called_expectAllPwmOutputsAreDisabled(void)
{
	ensureMotorFullyEnabled();
	CWG1STR = anyByteWithMaskClear(STEERING_MASK);
	uint8_t originalCwg1strWithClearSteering = CWG1STR;
	motorOn(anyEncoderCount());

	motorOff();
	TEST_ASSERT_EQUAL_UINT8(originalCwg1strWithClearSteering, CWG1STR);
}

void test_motorOff_calledWhenMotorAlreadyOff_expectMotorStoppedEventIsNotPublished(void)
{
	ensureMotorFullyEnabled();
	motorOff();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, onMotorStoppedCalls);
}

void test_motorOff_called_expectMotorStoppedEventIsPublishedWithSameRequestedCount(void)
{
	ensureMotorFullyEnabled();
	int16_t count = anyEncoderCount();
	motorOn(count);
	motorOff();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(count, onMotorStoppedArgs.requestedCount, "Count");
}

void test_motorOff_calledWhenTurningClockwise_expectMotorStoppedEventIsPublishedWithTmr1AsActualCount(void)
{
	ensureMotorFullyEnabled();
	int16_t count = anyClockwiseCount();
	motorOn(count);

	TMR1H = (uint8_t) ((count >> 8) & 0xff);
	TMR1L = (uint8_t) ((count >> 0) & 0xff);

	motorOff();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(count, onMotorStoppedArgs.actualCount, "Count");
}

void test_motorOff_calledWhenTurningAntiClockwise_expectMotorStoppedEventIsPublishedWithTmr1AsActualCount(void)
{
	ensureMotorFullyEnabled();
	int16_t count = anyAntiClockwiseCount();
	motorOn(count);

	int16_t negatedCount = -count;
	TMR1H = (uint8_t) ((negatedCount >> 8) & 0xff);
	TMR1L = (uint8_t) ((negatedCount >> 0) & 0xff);

	motorOff();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(count, onMotorStoppedArgs.actualCount, "Count");
}

void test_motorOff_calledWhenNoFaults_expectMotorStoppedEventIsPublishedWithClearFaults(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR4bits.TMR1IF = 0;
	PIR2bits.C1IF = 0;
	motorOff();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedCalls, "Calls");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, onMotorStoppedArgs.fault.all, "Fault");
}

void test_motorOff_calledWhenEncoderOverflowed_expectMotorStoppedEventIsPublishedWithEncoderOverflowFault(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR4bits.TMR1IF = 1;
	motorOff();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedCalls, "Calls");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedArgs.fault.encoderOverflow, "Fault");
}

void test_motorOff_calledWhenEncoderOverflowedAndAlreadyStopped_expectMotorStoppedEventIsNotPublished(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR4bits.TMR1IF = 1;
	CWG1STR &= ~STEERING_MASK;
	motorOff();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, onMotorStoppedCalls);
}

void test_motorOff_calledWhenEncoderOverflowed_expectTimer1InterruptFlagIsCleared(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR4bits.TMR1IF = 1;
	motorOff();
	TEST_ASSERT_FALSE(PIR4bits.TMR1IF);
}

void test_motorOff_calledWhenEncoderOverflowedAndAlreadyOff_expectTimer1InterruptFlagIsCleared(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR4bits.TMR1IF = 1;
	CWG1STR &= ~STEERING_MASK;
	motorOff();
	TEST_ASSERT_FALSE(PIR4bits.TMR1IF);
}

void test_motorOff_calledWhenCurrentLimited_expectMotorStoppedEventIsPublishedWithCurrentLimitedFault(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR2bits.C1IF = 1;
	motorOff();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedCalls, "Calls");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedArgs.fault.currentLimited, "Fault");
}

void test_motorOff_calledWhenCurrentLimitedAndAlreadyOff_expectMotorStoppedEventIsNotPublished(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR2bits.C1IF = 1;
	CWG1STR &= ~STEERING_MASK;
	motorOff();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, onMotorStoppedCalls);
}

void test_motorOff_calledWhenCurrentLimited_expectCurrentSenseComparatorInterruptFlagIsCleared(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR2bits.C1IF = 1;
	motorOff();
	TEST_ASSERT_FALSE(PIR2bits.C1IF);
}

void test_motorOff_calledWhenCurrentLimitedAndAlreadyOff_expectCurrentSenseComparatorInterruptFlagIsCleared(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR2bits.C1IF = 1;
	CWG1STR &= ~STEERING_MASK;
	motorOff();
	TEST_ASSERT_FALSE(PIR2bits.C1IF);
}

void test_motorOff_called_expectCwgShutdownInterruptFlagIsCleared(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR7bits.CWG1IF = 1;
	motorOff();
	TEST_ASSERT_FALSE(PIR7bits.CWG1IF);
}

void test_motorOff_calledWhenAlreadyOff_expectCwgShutdownInterruptFlagIsCleared(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR7bits.CWG1IF = 1;
	motorOff();
	CWG1STR &= ~STEERING_MASK;
	TEST_ASSERT_FALSE(PIR7bits.CWG1IF);
}

// TODO: motorOff() - PWM stops increasing (no more schedules)
// TODO: MOTOR_STOPPED when timeout - expect fault.encoderTimeout set
