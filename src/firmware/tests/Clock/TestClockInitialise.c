#include <xc.h>
#include <unity.h>

#include "../NonDeterminism.h"

#include "Clock.h"

TEST_FILE("Clock/ClockInitialise.c")

void setUp(void)
{
}

void tearDown(void)
{
}

void test_clockInitialise_called_expectOscillatorIsEnabled(void)
{
	OSCEN = anyByteWithMaskClear(_OSCEN_SOSCEN_MASK);
	uint8_t originalOscen = OSCEN;

	clockInitialise();
	TEST_ASSERT_EQUAL_UINT8(OSCEN | _OSCEN_SOSCEN_MASK, OSCEN);
}

void test_clockInitialise_called_expectOscillatorIsReadyToBeUsed(void)
{
	clockInitialise();
	TEST_ASSERT_TRUE(OSCSTATbits.SOR);
}

void test_clockInitialise_called_expectClockSourceIsOscillator(void)
{
	T0CON1 = anyByte();
	clockInitialise();

	int source = (T0CON1 & _T0CON1_T0CS_MASK) >> _T0CON1_T0CS_POSITION;
	TEST_ASSERT_EQUAL_INT(0b110, source & 0b111);
}

void test_clockInitialise_called_expectTimerIsAsynchronous(void)
{
	T0CON1 = anyByteWithMaskClear(_T0CON1_T0ASYNC_MASK);
	clockInitialise();
	TEST_ASSERT_TRUE(T0CON1bits.T0ASYNC);
}

void test_clockInitialise_called_expectTimerPrescalerIs32768ToProduce1SecondTicks(void)
{
	T0CON1 = anyByte();
	clockInitialise();

	int prescale = (T0CON1 & _T0CON1_T0CKPS_MASK) >> _T0CON1_T0CKPS_POSITION;
	TEST_ASSERT_EQUAL_INT(0b1111, prescale & 0b1111);
}

void test_clockInitialise_called_expectTimerIsEnabled(void)
{
	T0CON0 = anyByteWithMaskClear(_T0CON0_T0EN_MASK);
	clockInitialise();
	TEST_ASSERT_TRUE(T0CON0bits.T0EN);
}

void test_clockInitialise_called_expectTimerIsConfiguredAs8BitMode(void)
{
	T0CON0 = anyByteWithMaskSet(_T0CON0_T016BIT_MASK);
	clockInitialise();
	TEST_ASSERT_FALSE(T0CON0bits.T016BIT);
}

void test_clockInitialise_called_expectTimerPostscalerIs15Seconds(void)
{
	T0CON0 = anyByte();
	clockInitialise();

	int postscale = (T0CON0 & _T0CON0_T0OUTPS_MASK) >> _T0CON0_T0OUTPS_POSITION;
	TEST_ASSERT_EQUAL_INT(14, postscale & 0b1111);
}

void test_clockInitialise_called_expectTimerInterruptFlagIsClear(void)
{
	PIR0 = anyByteWithMaskSet(_PIR0_TMR0IF_MASK);
	uint8_t originalPir0 = PIR0;

	clockInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPir0 & ~_PIR0_TMR0IF_MASK, PIR0);
}

void test_clockInitialise_called_expectTimerCanWakeDeviceFromSleep(void)
{
	PIE0 = anyByteWithMaskClear(_PIE0_TMR0IE_MASK);
	uint8_t originalPie0 = PIE0;

	clockInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPie0 | _PIE0_TMR0IE_MASK, PIE0);
}

void test_clockInitialise_called_expectTimerIsZero(void)
{
	TMR0L = anyByteExcept(0);
	clockInitialise();
	TEST_ASSERT_EQUAL_UINT8(0, TMR0L);
}

void test_clockInitialise_called_expectTimerComparisonIs15Seconds(void)
{
	TMR0H = anyByteExcept(1);
	clockInitialise();
	TEST_ASSERT_EQUAL_UINT8(1, TMR0H);
}
