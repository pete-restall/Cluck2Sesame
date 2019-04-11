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
	clockInitialise();
	TEST_ASSERT_TRUE(OSCENbits.SOSCEN);
}

void test_clockInitialise_called_expectOscillatorIsReadyToBeUsed(void)
{
	clockInitialise();
	TEST_ASSERT_TRUE(OSCSTATbits.SOR);
}
