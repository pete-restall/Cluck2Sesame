#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Mock_Event.h"
#include "Mock_NearScheduler.h"
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
	voltageRegulatorInitialise();
	TEST_ASSERT_FALSE(TRISBbits.TRISB2);
}

void test_voltageRegulatorInitialise_called_expectEnablePinIsDigital(void)
{
	ANSELB = anyByteWithMaskSet(_ANSELB_ANSB2_MASK);
	voltageRegulatorInitialise();
	TEST_ASSERT_FALSE(ANSELBbits.ANSB2);
}

void test_voltageRegulatorInitialise_called_expectEnablePinIsLow(void)
{
	LATB = anyByteWithMaskSet(_LATB_LATB2_MASK);
	voltageRegulatorInitialise();
	TEST_ASSERT_FALSE(LATBbits.LATB2);
}

void test_voltageRegulatorInitialise_called_expectVmcuSelPinIsOutput(void)
{
	TRISB = anyByteWithMaskSet(_TRISB_TRISB0_MASK);
	voltageRegulatorInitialise();
	TEST_ASSERT_FALSE(TRISBbits.TRISB0);
}

void test_voltageRegulatorInitialise_called_expectVmcuSelPinIsDigital(void)
{
	ANSELB = anyByteWithMaskSet(_ANSELB_ANSB0_MASK);
	voltageRegulatorInitialise();
	TEST_ASSERT_FALSE(ANSELBbits.ANSB0);
}

void test_voltageRegulatorInitialise_called_expectVmcuSelPinIsLow(void)
{
	LATB = anyByteWithMaskSet(_LATB_LATB0_MASK);
	voltageRegulatorInitialise();
	TEST_ASSERT_FALSE(LATBbits.LATB0);
}

void test_voltageRegulatorInitialise_called_expectNonVoltageRegulatorTrisBitsAreUnchanged(void)
{
	static const uint8_t usedPins = _TRISB_TRISB0_MASK | _TRISB_TRISB2_MASK;
	TRISB = anyByte();
	uint8_t expectedTrisb = TRISB | usedPins;

	voltageRegulatorInitialise();
	TEST_ASSERT_EQUAL_HEX8(expectedTrisb, TRISB | usedPins);
}

void test_voltageRegulatorInitialise_called_expectNonVoltageRegulatorAnselBitsAreUnchanged(void)
{
	static const uint8_t usedPins = _ANSELB_ANSB0_MASK | _ANSELB_ANSB2_MASK;
	ANSELB = anyByte();
	uint8_t expectedAnselb = ANSELB | usedPins;

	voltageRegulatorInitialise();
	TEST_ASSERT_EQUAL_HEX8(expectedAnselb, ANSELB | usedPins);
}

void test_voltageRegulatorInitialise_called_expectNonVoltageRegulatorLatBitsAreUnchanged(void)
{
	static const uint8_t usedPins = _LATB_LATB0_MASK | _LATB_LATB2_MASK;
	LATB = anyByte();
	uint8_t expectedLatb = LATB | usedPins;

	voltageRegulatorInitialise();
	TEST_ASSERT_EQUAL_HEX8(expectedLatb, LATB | usedPins);
}
