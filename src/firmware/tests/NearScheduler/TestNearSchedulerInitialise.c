#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Mock_Event.h"
#include "NearScheduler.h"

#include "NonDeterminism.h"

TEST_FILE("NearScheduler.c")

void setUp(void)
{
}

void tearDown(void)
{
}

void test_MS_TO_TICKS_calledWithUpToTenMilliseconds_ExpectOneTick(void)
{
	for (uint8_t ms = 1; ms <= 10; ms++)
	{
		TEST_ASSERT_EQUAL_UINT8(1, MS_TO_TICKS(ms));
	}
}

void test_MS_TO_TICKS_calledWithExactMultiplesOfTenMilliseconds_ExpectNoRoundingIsPerformed(void)
{
	uint16_t ms;
	uint8_t ticks;
	for (ms = 0, ticks = 0; ms <= 255 * 10; ms += 10, ticks++)
	{
		TEST_ASSERT_EQUAL_UINT8(ticks, MS_TO_TICKS(ms));
	}
}

void test_MS_TO_TICKS_calledWithRemainderOfTenMillisecondMultiples_ExpectTicksRoundedUp(void)
{
	uint8_t ms = anyByte();
	uint8_t roundedUpTicks = ms / 10 + 1;
	uint8_t msMultipleOfTen = (ms / 10) * 10;
	for (uint8_t remainder = 1; remainder <= 9; remainder++)
	{
		TEST_ASSERT_EQUAL_UINT8(
			roundedUpTicks,
			MS_TO_TICKS(msMultipleOfTen + remainder));
	}
}

void test_nearSchedulerInitialise_called_expectNcoIsDisabled(void)
{
	NCO1CON = anyByteWithMaskSet(_NCO1CON_N1EN_MASK);
	nearSchedulerInitialise();
	TEST_ASSERT_FALSE(NCO1CONbits.N1EN);
}

void test_nearSchedulerInitialise_called_expectNcoPolarityIsNotInverted(void)
{
	NCO1CON = anyByteWithMaskSet(_NCO1CON_N1POL_MASK);
	nearSchedulerInitialise();
	TEST_ASSERT_FALSE(NCO1CONbits.N1POL);
}

void test_nearSchedulerInitialise_called_expectNcoModeIsFixedDutyCycle(void)
{
	NCO1CON = anyByteWithMaskSet(_NCO1CON_N1PFM_MASK);
	nearSchedulerInitialise();
	TEST_ASSERT_FALSE(NCO1CONbits.N1PFM);
}

void test_nearSchedulerInitialise_called_expectNcoPulseWidthBitsAreClear(void)
{
	NCO1CLK = anyByte();
	nearSchedulerInitialise();
	TEST_ASSERT_BITS_LOW(_NCO1CLK_N1PWS_MASK, NCO1CLK);
}

void test_nearSchedulerInitialise_called_expectNcoClockSourceIsSecondaryOscillator(void)
{
	NCO1CLK = anyByte();
	nearSchedulerInitialise();
	TEST_ASSERT_EQUAL_INT(0b0101, (NCO1CLK >> _NCO1CLK_N1CKS_POSITION) & 0b1111);
}

void test_nearSchedulerInitialise_called_expectNcoIncrementGivesOverflowPeriodOfTenMillisecondsWhenUsedWith32768HzCrystal(void)
{
	const uint16_t ncoIncrement = 3200;
	NCO1INCU = anyByte();
	NCO1INCH = anyByte();
	NCO1INCL = anyByte();
	nearSchedulerInitialise();

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, NCO1INCU, "NCO1INCU");

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		(ncoIncrement >> 8) & 0xff,
		NCO1INCH,
		"NCO1INCH");

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		ncoIncrement & 0xff,
		NCO1INCL,
		"NCO1INCL");
}

void test_nearSchedulerInitialise_called_expectNcoCanWakeFromSleep(void)
{
	PIE7 = anyByteWithMaskClear(_PIE7_NCO1IE_MASK);
	uint8_t originalPie7 = PIE7;
	nearSchedulerInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPie7 | _PIE7_NCO1IE_MASK, PIE7);
}

void test_nearSchedulerInitialise_called_expectNcoInterruptFlagIsClear(void)
{
	PIR7 = anyByteWithMaskSet(_PIR7_NCO1IF_MASK);
	uint8_t originalPir7 = PIR7;
	nearSchedulerInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPir7 & ~_PIR7_NCO1IF_MASK, PIR7);
}
