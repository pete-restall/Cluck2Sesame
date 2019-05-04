#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "PwmTimer.h"

#include "NonDeterminism.h"

TEST_FILE("PwmTimer.c")

void setUp(void)
{
}

void tearDown(void)
{
}

void test_pwmTimerInitialise_called_expectTimerClockSourceIsInstructionClock(void)
{
	T2CLKCON = anyByte();
	pwmTimerInitialise();
	TEST_ASSERT_EQUAL_UINT8(1, T2CLKCON);
}

void test_pwmTimerInitialise_called_expectTimerIsDisabled(void)
{
	T2CON = anyByteWithMaskSet(_T2CON_ON_MASK);
	pwmTimerInitialise();
	TEST_ASSERT_FALSE(T2CONbits.ON);
}

void test_pwmTimerInitialise_called_expectTimerPrescalerOfTwo(void)
{
	T2CON = anyByte();
	pwmTimerInitialise();
	TEST_ASSERT_EQUAL_UINT8(
		0b001 << _T2CON_CKPS_POSITION,
		T2CON & _T2CON_CKPS_MASK);
}

void test_pwmTimerInitialise_called_expectTimerPostscalerOfEight(void)
{
	T2CON = anyByte();
	pwmTimerInitialise();
	TEST_ASSERT_EQUAL_UINT8(
		7 << _T2CON_OUTPS_POSITION,
		T2CON & _T2CON_OUTPS_MASK);
}

void test_pwmTimerInitialise_called_expectNoTimerPrescalerSynchronisation(void)
{
	T2HLT = anyByteWithMaskSet(_T2HLT_PSYNC_MASK);
	pwmTimerInitialise();
	TEST_ASSERT_FALSE(T2HLTbits.PSYNC);
}

void test_pwmTimerInitialise_called_expectTimerActiveEdgeIsRising(void)
{
	T2HLT = anyByteWithMaskSet(_T2HLT_CKPOL_MASK);
	pwmTimerInitialise();
	TEST_ASSERT_FALSE(T2HLTbits.CKPOL);
}

void test_pwmTimerInitialise_called_expectTimerEnableSynchronisation(void)
{
	T2HLT = anyByteWithMaskClear(_T2HLT_CKSYNC_MASK);
	pwmTimerInitialise();
	TEST_ASSERT_TRUE(T2HLTbits.CKSYNC);
}

void test_pwmTimerInitialise_called_expectTimerModeIsFreeRunningSoftwareGated(void)
{
	T2HLT = anyByteWithMaskSet(_T2HLT_MODE_MASK);
	pwmTimerInitialise();
	TEST_ASSERT_EQUAL_UINT8(0, T2HLT & _T2HLT_MODE_MASK);
}

void test_pwmTimerInitialise_called_expectPeriodResultsIn62500HzPwm(void)
{
	PR2 = anyByte();
	pwmTimerInitialise();
	TEST_ASSERT_EQUAL_UINT8(63, PR2);
}

void test_pwmTimerInitialise_called_expectTimerInterruptFlagIsCleared(void)
{
	PIR4 = anyByteWithMaskSet(_PIR4_TMR2IF_MASK);
	uint8_t originalPir4 = PIR4;
	pwmTimerInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPir4 & ~_PIR4_TMR2IF_MASK, PIR4);
}

void test_pwmTimerInitialise_called_expectTimerInterruptWakesDeviceFromSleep(void)
{
	PIR4 = anyByteWithMaskClear(_PIE4_TMR2IE_MASK);
	uint8_t originalPie4 = PIE4;
	pwmTimerInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPie4 | _PIE4_TMR2IE_MASK, PIE4);
}
