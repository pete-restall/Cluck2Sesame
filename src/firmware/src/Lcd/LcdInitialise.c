#include <xc.h>
#include <stdint.h>

#include "Event.h"
#include "NvmSettings.h"
#include "VoltageRegulator.h"
#include "Lcd.h"

#define PORTA_PIN_MASK 0b00000011
#define PORTC_PIN_MASK 0b11001111

#define PPS_OUT_PWM3 0x0b
#define PPS_OUT_PWM5 0x0d

void lcdInitialise(void)
{
	ANSELA &= PORTA_PIN_MASK;
	LATA &= PORTA_PIN_MASK;
	ANSELC &= PORTC_PIN_MASK;
	LATC &= PORTC_PIN_MASK;
	TRISA &= PORTA_PIN_MASK;
	TRISC &= PORTC_PIN_MASK;

	PWM5CON = 0;
	PWM5DCH = (nvmSettings.lcd.contrast >> 2) & 0b00111111;
	PWM5DCL = nvmSettings.lcd.contrast << 6;
	RA2PPS = PPS_OUT_PWM5;

	PWM3CON = 0;
	PWM3DCH = (nvmSettings.lcd.backlightBrightness >> 2) & 0b00111111;
	PWM3DCL = nvmSettings.lcd.backlightBrightness << 6;
	RC5PPS = PPS_OUT_PWM3;

	lcdEnableCount = 0;

	static const struct EventSubscription onVoltageRegulatorEnabledSubscription =
	{
		.type = VOLTAGE_REGULATOR_ENABLED,
		.handler = &onVoltageRegulatorEnabled,
		.state = (void *) 0
	};

	eventSubscribe(&onVoltageRegulatorEnabledSubscription);
}
