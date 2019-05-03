#include <xc.h>
#include <stdint.h>

#include "NvmSettings.h"
#include "Lcd.h"

#define PORTA_PIN_MASK 0b00000011
#define PORTC_PIN_MASK 0b11001111

#define PPS_OUT_PWM3 0x0b

void lcdInitialise(void)
{
	ANSELA &= PORTA_PIN_MASK;
	LATA &= PORTA_PIN_MASK;
	ANSELC &= PORTC_PIN_MASK;
	LATC &= PORTC_PIN_MASK;
	TRISA &= PORTA_PIN_MASK;
	TRISC &= PORTC_PIN_MASK;
	RC5PPS = PPS_OUT_PWM3;

	PWM3CON = 0;
	PWM3DCH = nvmSettings.lcd.backlightBrightness;
	PWM3DCL = 0;
}
