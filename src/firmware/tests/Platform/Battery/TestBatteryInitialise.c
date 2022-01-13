#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Mock_Event.h"
#include "Platform/Battery.h"

#include "BatteryFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Battery.c")

void onBeforeTest(void)
{
	batteryFixtureSetUp();
}

void onAfterTest(void)
{
	batteryFixtureTearDown();
}

void test_batteryInitialise_called_expectChargerEnablePinIsOutput(void)
{
	TRISB = anyByteWithMaskSet(_TRISB_TRISB3_MASK);
	batteryInitialise();
	TEST_ASSERT_FALSE(TRISBbits.TRISB3);
}

void test_batteryInitialise_called_expectChargerEnablePinIsDigital(void)
{
	ANSELB = anyByteWithMaskSet(_ANSELB_ANSB3_MASK);
	batteryInitialise();
	TEST_ASSERT_FALSE(ANSELBbits.ANSB3);
}

void test_batteryInitialise_called_expectChargerEnablePinIsLow(void)
{
	LATB = anyByteWithMaskSet(_LATB_LATB3_MASK);
	batteryInitialise();
	TEST_ASSERT_FALSE(LATBbits.LATB3);
}

void test_batteryInitialise_called_expectChargerGoodPinIsInput(void)
{
	TRISB = anyByteWithMaskClear(_TRISB_TRISB5_MASK);
	batteryInitialise();
	TEST_ASSERT_TRUE(TRISBbits.TRISB5);
}

void test_batteryInitialise_called_expectChargerGoodPinIsDigital(void)
{
	ANSELB = anyByteWithMaskSet(_ANSELB_ANSB5_MASK);
	batteryInitialise();
	TEST_ASSERT_FALSE(ANSELBbits.ANSB5);
}

void test_batteryInitialise_called_expectChargerGoodPinUsesTtlThresholds(void)
{
	INLVLB = anyByteWithMaskSet(_INLVLB_INLVLB5_MASK);
	batteryInitialise();
	TEST_ASSERT_FALSE(INLVLBbits.INLVLB5);
}

void test_batteryInitialise_called_expectChargingPinIsInput(void)
{
	TRISB = anyByteWithMaskClear(_TRISB_TRISB4_MASK);
	batteryInitialise();
	TEST_ASSERT_TRUE(TRISBbits.TRISB4);
}

void test_batteryInitialise_called_expectChargingPinIsDigital(void)
{
	ANSELB = anyByteWithMaskSet(_ANSELB_ANSB4_MASK);
	batteryInitialise();
	TEST_ASSERT_FALSE(ANSELBbits.ANSB4);
}

void test_batteryInitialise_called_expectChargingPinUsesTtlThresholds(void)
{
	INLVLB = anyByteWithMaskSet(_INLVLB_INLVLB4_MASK);
	batteryInitialise();
	TEST_ASSERT_FALSE(INLVLBbits.INLVLB4);
}

void test_batteryInitialise_called_expectNonChargerTrisBitsAreUnchanged(void)
{
	static const uint8_t usedPins = _TRISB_TRISB3_MASK | _TRISB_TRISB4_MASK | _TRISB_TRISB5_MASK;
	TRISB = anyByte();
	uint8_t expectedTrisb = TRISB | usedPins;

	batteryInitialise();
	TEST_ASSERT_EQUAL_HEX8(expectedTrisb, TRISB | usedPins);
}

void test_batteryInitialise_called_expectNonChargerAnselBitsAreUnchanged(void)
{
	static const uint8_t usedPins = _ANSELB_ANSB3_MASK | _ANSELB_ANSB4_MASK | _ANSELB_ANSB5_MASK;
	ANSELB = anyByte();
	uint8_t expectedAnselb = ANSELB | usedPins;

	batteryInitialise();
	TEST_ASSERT_EQUAL_HEX8(expectedAnselb, ANSELB | usedPins);
}

void test_batteryInitialise_called_expectNonChargerLatBitsAreUnchanged(void)
{
	static const uint8_t usedPins = _LATB_LATB3_MASK | _LATB_LATB4_MASK | _LATB_LATB5_MASK;
	LATB = anyByte();
	uint8_t expectedLatb = LATB | usedPins;

	batteryInitialise();
	TEST_ASSERT_EQUAL_HEX8(expectedLatb, LATB | usedPins);
}

void test_batteryInitialise_called_expectNonChargerInputPinThresholdsAreUnchanged(void)
{
	static const uint8_t usedPins = _INLVLB_INLVLB4_MASK | _INLVLB_INLVLB5_MASK;
	INLVLB = anyByte();
	uint8_t expectedInlvlb = INLVLB | usedPins;

	batteryInitialise();
	TEST_ASSERT_EQUAL_HEX8(expectedInlvlb, INLVLB | usedPins);
}

void test_batteryInitialise_called_expectIocModuleIsEnabled(void)
{
	PMD0 = anyByteWithMaskSet(_PMD0_IOCMD_MASK);
	uint8_t expectedPmd0 = PMD0 & ~_PMD0_IOCMD_MASK;

	batteryInitialise();
	TEST_ASSERT_EQUAL_HEX8(expectedPmd0, PMD0);
}

void test_batteryInitialise_called_expectIocInterruptsAreEnabled(void)
{
	PIE0 = anyByteWithMaskClear(_PIE0_IOCIE_MASK);
	uint8_t expectedPie0 = PIE0 | _PIE0_IOCIE_MASK;

	batteryInitialise();
	TEST_ASSERT_EQUAL_HEX8(expectedPie0, PIE0);
}

void test_batteryInitialise_called_expectIocIsEnabledForPositiveEdgeOnChargerGoodPin(void)
{
	IOCBP = anyByteWithMaskClear(_IOCBP_IOCBP5_MASK);
	uint8_t expectedIocbp = IOCBP | _IOCBP_IOCBP5_MASK;

	batteryInitialise();
	TEST_ASSERT_EQUAL_HEX8(expectedIocbp, IOCBP);
}

void test_batteryInitialise_called_expectIocIsEnabledForNegativeEdgeOnChargerGoodPin(void)
{
	IOCBN = anyByteWithMaskClear(_IOCBN_IOCBN5_MASK);
	uint8_t expectedIocbn = IOCBN | _IOCBN_IOCBN5_MASK;

	batteryInitialise();
	TEST_ASSERT_EQUAL_HEX8(expectedIocbn, IOCBN);
}
