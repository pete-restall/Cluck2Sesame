#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/NvmSettings.h"
#include "Mock_VoltageRegulator.h"
#include "Mock_LcdInternals.h"
#include "Platform/Lcd.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Event.c")
TEST_FILE("Platform/Lcd/LcdInitialise.c")
TEST_FILE("Platform/Lcd/LcdEnableDisable.c")

void onBeforeTest(void)
{
}

void onAfterTest(void)
{
}

void test_lcdInitialise_called_expectPwm3And5ModulesAreEnabled(void)
{
	PMD3 = anyByteWithMaskSet(_PMD3_PWM3MD_MASK | _PMD3_PWM5MD_MASK);
	uint8_t originalPmd3 = PMD3;
	lcdInitialise();
	TEST_ASSERT_EQUAL_UINT8(
		originalPmd3 & ~(_PMD3_PWM3MD_MASK | _PMD3_PWM5MD_MASK),
		PMD3);
}

void test_lcdInitialise_called_expectLcdContrastPinIsTristated(void)
{
	static const uint8_t lcdContrastPin = _TRISA_TRISA2_MASK;
	ANSELA = anyByteWithMaskClear(lcdContrastPin);
	TRISA = anyByteWithMaskClear(lcdContrastPin);
	lcdInitialise();
	TEST_ASSERT_BITS_HIGH_MESSAGE(lcdContrastPin, ANSELA, "ANSELA");
	TEST_ASSERT_BITS_HIGH_MESSAGE(lcdContrastPin, TRISA, "TRISA");
}

void test_lcdInitialise_called_expectLcdPortAPinsExceptContrastPinAreAllOutputs(void)
{
	static const uint8_t lcdContrastPin = _TRISA_TRISA3_MASK;
	static const uint8_t usedPins =
		_TRISA_TRISA3_MASK |
		_TRISA_TRISA4_MASK |
		_TRISA_TRISA5_MASK |
		_TRISA_TRISA6_MASK |
		_TRISA_TRISA7_MASK;

	TRISA = anyByteWithMaskSet(usedPins);
	uint8_t originalTrisa = TRISA;
	lcdInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalTrisa & ~usedPins, TRISA & ~lcdContrastPin);
}

void test_lcdInitialise_called_expectLcdPortCPinsAreAllOutputs(void)
{
	static const uint8_t usedPins =
		_TRISC_TRISC4_MASK |
		_TRISC_TRISC5_MASK;

	TRISC = anyByteWithMaskSet(usedPins);
	uint8_t originalTrisc = TRISC;
	lcdInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalTrisc & ~usedPins, TRISC);
}

void test_lcdInitialise_called_expectLcdPortAPinsExceptContrastPinAreAllDigital(void)
{
	static const uint8_t usedPins =
		_ANSELA_ANSA3_MASK |
		_ANSELA_ANSA4_MASK |
		_ANSELA_ANSA5_MASK |
		_ANSELA_ANSA6_MASK |
		_ANSELA_ANSA7_MASK;

	ANSELA = anyByteWithMaskSet(usedPins);
	uint8_t originalAnsela = ANSELA;
	lcdInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalAnsela & ~usedPins, ANSELA);
}

void test_lcdInitialise_called_expectLcdPortCPinsAreAllDigital(void)
{
	static const uint8_t usedPins =
		_ANSELC_ANSC4_MASK |
		_ANSELC_ANSC5_MASK;

	ANSELC = anyByteWithMaskSet(usedPins);
	uint8_t originalAnselc = ANSELC;
	lcdInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalAnselc & ~usedPins, ANSELC);
}

void test_lcdInitialise_called_expectLcdPortAPinsAreAllLow(void)
{
	static const uint8_t usedPins =
		_LATA_LATA2_MASK |
		_LATA_LATA3_MASK |
		_LATA_LATA4_MASK |
		_LATA_LATA5_MASK |
		_LATA_LATA6_MASK |
		_LATA_LATA7_MASK;

	LATA = anyByteWithMaskSet(usedPins);
	uint8_t originalLata = LATA;
	lcdInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalLata & ~usedPins, LATA);
}

void test_lcdInitialise_called_expectLcdPortCPinsAreAllLow(void)
{
	static const uint8_t usedPins =
		_LATC_LATC4_MASK |
		_LATC_LATC5_MASK;

	LATC = anyByteWithMaskSet(usedPins);
	uint8_t originalLatc = LATC;
	lcdInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalLatc & ~usedPins, LATC);
}

void test_lcdInitialise_called_expectContrastPinIsPwm5Output(void)
{
	const uint8_t pwm5 = 0x0d;
	RA2PPS = anyByteExcept(pwm5);
	lcdInitialise();
	TEST_ASSERT_EQUAL_UINT8(pwm5, RA2PPS);
}

void test_lcdInitialise_called_expectContrastIsDefaultSetting(void)
{
	PWM5DCH = anyByte();
	PWM5DCL = anyByte();
	lcdInitialise();

	uint8_t high = (nvmSettings.platform.lcd.contrast >> 2) & 0b00111111;
	uint8_t low = (nvmSettings.platform.lcd.contrast << 6) & 0b11000000;
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(high, PWM5DCH, "H");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(low, PWM5DCL, "L");
}

void test_lcdInitialise_called_expectContrastPwmIsDisabled(void)
{
	PWM5CON = anyByteWithMaskSet(_PWM5CON_PWM5EN_MASK);
	lcdInitialise();
	TEST_ASSERT_FALSE(PWM5CONbits.PWM5EN);
}

void test_lcdInitialise_called_expectContrastPwmIsActiveHigh(void)
{
	PWM5CON = anyByteWithMaskSet(_PWM5CON_PWM5POL_MASK);
	lcdInitialise();
	TEST_ASSERT_FALSE(PWM5CONbits.PWM5POL);
}

void test_lcdInitialise_called_expectBacklightPinIsPwm3Output(void)
{
	const uint8_t pwm3 = 0x0b;
	RC5PPS = anyByteExcept(pwm3);
	lcdInitialise();
	TEST_ASSERT_EQUAL_UINT8(pwm3, RC5PPS);
}

void test_lcdInitialise_called_expectBacklightBrightnessIsDefaultSetting(void)
{
	PWM3DCH = anyByte();
	PWM3DCL = anyByte();
	lcdInitialise();

	uint8_t high = (nvmSettings.platform.lcd.backlightBrightness >> 2) & 0b00111111;
	uint8_t low = (nvmSettings.platform.lcd.backlightBrightness << 6) & 0b11000000;
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(high, PWM3DCH, "H");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(low, PWM3DCL, "L");
}

void test_lcdInitialise_called_expectBacklightPwmIsDisabled(void)
{
	PWM3CON = anyByteWithMaskSet(_PWM3CON_PWM3EN_MASK);
	lcdInitialise();
	TEST_ASSERT_FALSE(PWM3CONbits.PWM3EN);
}

void test_lcdInitialise_called_expectBacklightPwmIsActiveHigh(void)
{
	PWM3CON = anyByteWithMaskSet(_PWM3CON_PWM3POL_MASK);
	lcdInitialise();
	TEST_ASSERT_FALSE(PWM3CONbits.PWM3POL);
}
