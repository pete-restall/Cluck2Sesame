#include <xc.h>
#include <unity.h>

#include "Mock_NvmSettings.h"
#include "Lcd.h"

#include "NonDeterminism.h"

TEST_FILE("Lcd/LcdInitialise.c")

void setUp(void)
{
}

void tearDown(void)
{
}

void test_lcdInitialise_called_expectLcdPortAPinsAreAllOutputs(void)
{
	static const uint8_t usedPins =
		_TRISA_TRISA2_MASK |
		_TRISA_TRISA3_MASK |
		_TRISA_TRISA4_MASK |
		_TRISA_TRISA5_MASK |
		_TRISA_TRISA6_MASK |
		_TRISA_TRISA7_MASK;

	TRISA = anyByteWithMaskSet(usedPins);
	uint8_t originalTrisa = TRISA;
	lcdInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalTrisa & ~usedPins, TRISA);
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

void test_lcdInitialise_called_expectLcdPortAPinsAreAllDigital(void)
{
	static const uint8_t usedPins =
		_ANSELA_ANSA2_MASK |
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

void test_lcdInitialise_called_expectdBacklightPinIsPwm3Output(void)
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
	TEST_ASSERT_EQUAL_UINT8(nvmSettings.lcd.backlightBrightness, PWM3DCH);
	TEST_ASSERT_EQUAL_UINT8(0, PWM3DCL);
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
