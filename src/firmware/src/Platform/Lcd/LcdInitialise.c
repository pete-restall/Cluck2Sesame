#include <xc.h>
#include <stdint.h>

#include "../Event.h"
#include "../NvmSettings.h"
#include "../VoltageRegulator.h"
#include "Lcd.h"

#define PORTA_PIN_MASK 0b00000011
#define PORTC_PIN_MASK 0b11001111

#define PPS_OUT_PWM3 0x0b
#define PPS_OUT_PWM5 0x0d

struct LcdState lcdState;

static void buggyCompilerWorkaround(void)
{
	static const struct LcdPutsTransaction dummy =
	{
		.buffer = _OMNITARGET,
		.state = _OMNITARGET
	};

	lcdState.transaction.state = _OMNITARGET;
}

void lcdInitialise(void)
{
	// TODO: THE NVM_SETTINGS_CHANGED EVENT SHOULD BE HANDLED
	PMD3bits.PWM3MD = 0;
	PMD3bits.PWM5MD = 0;

	ANSELA &= PORTA_PIN_MASK;
	LATA &= PORTA_PIN_MASK;
	ANSELC &= PORTC_PIN_MASK;
	LATC &= PORTC_PIN_MASK;
	TRISA &= PORTA_PIN_MASK;
	TRISC &= PORTC_PIN_MASK;

	PWM5CON = 0;
	PWM5DCH = (nvmSettings.platform.lcd.contrast >> 2) & 0b00111111;
	PWM5DCL = nvmSettings.platform.lcd.contrast << 6;
	RA2PPS = PPS_OUT_PWM5;
	ANSELAbits.ANSA2 = 1;
	TRISAbits.TRISA2 = 1;

	PWM3CON = 0;
	PWM3DCH = (nvmSettings.platform.lcd.backlightBrightness >> 2) & 0b00111111;
	PWM3DCL = nvmSettings.platform.lcd.backlightBrightness << 6;
	RC5PPS = PPS_OUT_PWM3;

	lcdState.enableCount = 0;
	lcdState.flags.all = 0;
	lcdState.flags.isBusy = 1;

	static const struct EventSubscription onVoltageRegulatorEnabledSubscription =
	{
		.type = VOLTAGE_REGULATOR_ENABLED,
		.handler = &lcdOnVoltageRegulatorEnabled,
		.state = (void *) 0
	};

	eventSubscribe(&onVoltageRegulatorEnabledSubscription);

	static const struct EventSubscription onVoltageRegulatorDisabledSubscription =
	{
		.type = VOLTAGE_REGULATOR_DISABLED,
		.handler = &lcdOnVoltageRegulatorDisabled,
		.state = (void *) 0
	};

	eventSubscribe(&onVoltageRegulatorDisabledSubscription);
}
