#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Mock_Nvm.h"
#include "Mock_HexDigits.h"
#include "Mock_PeriodicMonitor.h"
#include "Platform/CalibrationMode.h"

#include "CalibrationModeFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"
#include "../../NvmSettingsFixture.h"

TEST_FILE("Platform/CalibrationMode.c")

void onBeforeTest(void)
{
	calibrationModeFixtureSetUp();
}

void onAfterTest(void)
{
	calibrationModeFixtureTearDown();
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectSubscriptionToWokenFromSleep(void)
{
	stubNvmSettingsWithCalibrationRequired();
	calibrationModeInitialise();
	assertWokenFromSleepSubscription();
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectNoSubscriptionToWokenFromSleep(void)
{
	stubNvmSettingsWithoutCalibrationRequired();
	calibrationModeInitialise();
	assertNoWokenFromSleepSubscription();
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectClockReferenceModuleIsEnabled(void)
{
	stubNvmSettingsWithCalibrationRequired();
	PMD0 = anyByteWithMaskSet(_PMD0_CLKRMD_MASK);
	uint8_t originalPmd0 = PMD0;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPmd0 & ~_PMD0_CLKRMD_MASK, PMD0);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectClockReferenceModuleIsDisabled(void)
{
	stubNvmSettingsWithoutCalibrationRequired();
	PMD0 = anyByteWithMaskClear(_PMD0_CLKRMD_MASK);
	uint8_t originalPmd0 = PMD0;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPmd0 | _PMD0_CLKRMD_MASK, PMD0);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectIcspPinsAreDigital(void)
{
	stubNvmSettingsWithCalibrationRequired();

	static const uint8_t usedPins = _ANSELB_ANSB6_MASK | _ANSELB_ANSB7_MASK;
	ANSELB = anyByteWithMaskSet(usedPins);
	uint8_t originalAnselb = ANSELB;

	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalAnselb & ~usedPins, ANSELB);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectIcspPinsAreDigital(void)
{
	stubNvmSettingsWithoutCalibrationRequired();

	static const uint8_t usedPins = _ANSELB_ANSB6_MASK | _ANSELB_ANSB7_MASK;
	ANSELB = anyByteWithMaskSet(usedPins);
	uint8_t originalAnselb = ANSELB;

	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalAnselb & ~usedPins, ANSELB);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectIcspPgcPinIsOutput(void)
{
	stubNvmSettingsWithCalibrationRequired();
	TRISB = anyByteWithMaskSet(_TRISB_TRISB6_MASK);
	uint8_t originalTrisb = TRISB & ~_TRISB_TRISB7_MASK;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalTrisb & ~_TRISB_TRISB6_MASK, TRISB & ~_TRISB_TRISB7_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectIcspPgcPinIsOutput(void)
{
	stubNvmSettingsWithoutCalibrationRequired();
	TRISB = anyByteWithMaskSet(_TRISB_TRISB6_MASK);
	uint8_t originalTrisb = TRISB & ~_TRISB_TRISB7_MASK;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalTrisb & ~_TRISB_TRISB6_MASK, TRISB & ~_TRISB_TRISB7_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectIcspPgcPinIsMappedToReferenceClockOutput(void)
{
	stubNvmSettingsWithCalibrationRequired();
	RB6PPS = anyByteExcept(0x1b);
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(0x1b, RB6PPS);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectIcspPgcPinIsNotMappedToAnyPeripheralOutput(void)
{
	stubNvmSettingsWithoutCalibrationRequired();
	RB6PPS = anyByteExcept(0);
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(0, RB6PPS);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectIcspPgdPinIsOutput(void)
{
	stubNvmSettingsWithCalibrationRequired();
	TRISB = anyByteWithMaskSet(_TRISB_TRISB7_MASK);
	uint8_t originalTrisb = TRISB & ~_TRISB_TRISB6_MASK;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalTrisb & ~_TRISB_TRISB7_MASK, TRISB & ~_TRISB_TRISB6_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectIcspPgdPinIsOutput(void)
{
	stubNvmSettingsWithoutCalibrationRequired();
	TRISB = anyByteWithMaskSet(_TRISB_TRISB7_MASK);
	uint8_t originalTrisb = TRISB & ~_TRISB_TRISB6_MASK;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalTrisb & ~_TRISB_TRISB7_MASK, TRISB & ~_TRISB_TRISB6_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectIcspPgdPinIsOpenDrain(void)
{
	stubNvmSettingsWithCalibrationRequired();
	ODCONB = anyByteWithMaskClear(_ODCONB_ODCB7_MASK);
	uint8_t originalOdconb = ODCONB;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalOdconb | _ODCONB_ODCB7_MASK, ODCONB);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectIcspPgdPinIsNotOpenDrain(void)
{
	stubNvmSettingsWithoutCalibrationRequired();
	ODCONB = anyByteWithMaskSet(_ODCONB_ODCB7_MASK);
	uint8_t originalOdconb = ODCONB;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalOdconb & ~_ODCONB_ODCB7_MASK, ODCONB);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectIcspPgdPinIsMappedToUart1TxAndRx(void)
{
	static const uint8_t ppsOutUart1Tx = 0x0f;
	static const uint8_t ppsInRb7 = 0x0f;
	stubNvmSettingsWithCalibrationRequired();
	RB7PPS = anyByteExcept(0x10);
	RX1DTPPS = anyByte();
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(ppsOutUart1Tx, RB7PPS, "TX");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(ppsInRb7, RX1DTPPS, "RX");
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectIcspPgdPinIsNotMappedToAnyPeripheralOutput(void)
{
	stubNvmSettingsWithoutCalibrationRequired();
	RB7PPS = anyByteExcept(0);
	RX1DTPPS = anyByteExcept(0);
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, RB7PPS, "TX");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, RX1DTPPS, "RX");
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectIcspPinsAreLow(void)
{
	stubNvmSettingsWithCalibrationRequired();
	LATB = anyByteWithMaskSet(_LATB_LATB6_MASK | _LATB_LATB7_MASK);
	uint8_t originalLatb = LATB;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalLatb & ~(_LATB_LATB6_MASK | _LATB_LATB7_MASK), LATB);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectIcspPinsAreLow(void)
{
	stubNvmSettingsWithoutCalibrationRequired();
	LATB = anyByteWithMaskSet(_LATB_LATB6_MASK | _LATB_LATB7_MASK);
	uint8_t originalLatb = LATB;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalLatb & ~(_LATB_LATB6_MASK | _LATB_LATB7_MASK), LATB);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectReferenceClockSourceIs32768HzCrystal(void)
{
	stubNvmSettingsWithCalibrationRequired();
	CLKRCLK = anyByte();
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(0b0101 << _CLKRCLK_CLKRCLK_POSITION, CLKRCLK & _CLKRCLK_CLKRCLK_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectReferenceClockDutyCycleIs50Percent(void)
{
	stubNvmSettingsWithCalibrationRequired();
	CLKRCON = anyByte();
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(0b10 << _CLKRCON_CLKRDC_POSITION, CLKRCON & _CLKRCON_CLKRDC_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectReferenceClockIsNotDivided(void)
{
	stubNvmSettingsWithCalibrationRequired();
	CLKRCON = anyByte();
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(0b000 << _CLKRCON_CLKRDIV_POSITION, CLKRCON & _CLKRCON_CLKRDIV_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectReferenceClockIsNotEnabled(void)
{
	stubNvmSettingsWithCalibrationRequired();
	CLKRCON = anyByteWithMaskSet(_CLKRCON_CLKREN_MASK);
	calibrationModeInitialise();
	TEST_ASSERT_BIT_LOW(_CLKRCON_CLKREN_POSITION, CLKRCON);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectUart1ModuleIsEnabled(void)
{
	stubNvmSettingsWithCalibrationRequired();
	PMD4 = anyByteWithMaskSet(_PMD4_UART1MD_MASK);
	uint8_t originalPmd4 = PMD4;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPmd4 & ~_PMD4_UART1MD_MASK, PMD4);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectUart1ModuleIsDisabled(void)
{
	stubNvmSettingsWithoutCalibrationRequired();
	PMD4 = anyByteWithMaskClear(_PMD4_UART1MD_MASK);
	uint8_t originalPmd4 = PMD4;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPmd4 | _PMD4_UART1MD_MASK, PMD4);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectUart1UsesAsynchronous8bitModeWithLowBaudMultiplierAndEnabledTransmitter(void)
{
	stubNvmSettingsWithCalibrationRequired();
	TX1STA = anyByte();
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT(_TX1STA_TXEN_MASK, TX1STA & ~_TX1STA_TRMT_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectUart1BaudRateIs9600bps(void)
{
	stubNvmSettingsWithCalibrationRequired();
	SP1BRG = anyWord();
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT16(51, SP1BRG);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectUart1Uses8bitModeAndIsEnabledForContinuousReception(void)
{
	static const uint8_t readonlyBits = _RC1STA_FERR_MASK | _RC1STA_OERR_MASK | _RC1STA_RX9D_MASK;
	stubNvmSettingsWithCalibrationRequired();
	RC1STA = anyByte();
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(_RC1STA_SPEN_MASK | _RC1STA_CREN_MASK, RC1STA & ~readonlyBits);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectUart1Uses8bitBaudRateGeneratorWithIdleHighTx(void)
{
	stubNvmSettingsWithCalibrationRequired();
	BAUD1CON = anyByte();
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(0, BAUD1CON & ~_BAUD1CON_RCIDL_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectUart1TxInterruptIsDisabled(void)
{
	stubNvmSettingsWithCalibrationRequired();
	PIE3 = anyByteWithMaskSet(_PIE3_TX1IE_MASK);
	uint8_t originalPie3 = PIE3 & ~_PIE3_RC1IE_MASK;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPie3 & ~_PIE3_TX1IE_MASK, PIE3 & ~_PIE3_RC1IE_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectUart1RxInterruptIsEnabled(void)
{
	stubNvmSettingsWithCalibrationRequired();
	PIE3 = anyByteWithMaskClear(_PIE3_RC1IE_MASK);
	uint8_t originalPie3 = PIE3 & ~_PIE3_TX1IE_MASK;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPie3 | _PIE3_RC1IE_MASK, PIE3 & ~_PIE3_TX1IE_MASK);
}
