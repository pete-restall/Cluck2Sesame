#include <xc.h>
#include <unity.h>

#include "Mock_Event.h"
#include "VoltageRegulator.h"

#include "NonDeterminism.h"

TEST_FILE("VoltageRegulator.c")

void setUp(void)
{
}

void tearDown(void)
{
}

void test_voltageRegulatorInitialise_called_expectEnablePinIsOutput(void)
{
	TRISB = anyByteWithMaskSet(_TRISB_TRISB2_MASK);
	uint8_t originalTrisb = TRISB;

	voltageRegulatorInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalTrisb & ~_TRISB_TRISB2_MASK, TRISB);
}

void test_voltageRegulatorInitialise_called_expectEnablePinIsDigital(void)
{
	ANSELB = anyByteWithMaskSet(_ANSELB_ANSB2_MASK);
	uint8_t originalAnselb = ANSELB;

	voltageRegulatorInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalAnselb & ~_ANSELB_ANSB2_MASK, ANSELB);
}

void test_voltageRegulatorInitialise_called_expectEnablePinIsLow(void)
{
	LATB = anyByteWithMaskSet(_LATB_LATB2_MASK);
	uint8_t originalLatb = LATB;

	voltageRegulatorInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalLatb & ~_LATB_LATB2_MASK, LATB);
}
