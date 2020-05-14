#include <xc.h>
#include <stdint.h>

#include "../VoltageRegulator.h"
#include "../PwmTimer.h"

#include "Lcd.h"

void lcdEnable(void)
{
	voltageRegulatorEnable();
	pwmTimerEnable();
	if (++lcdState.enableCount == 1 && voltageRegulatorIsEnabled())
		lcdConfigure();
}

void lcdOnVoltageRegulatorEnabled(const struct Event *event)
{
	if (lcdState.enableCount == 0)
	{
		ANSELAbits.ANSA2 = 1;
		TRISAbits.TRISA2 = 1;
	}
	else
		lcdConfigure();
}

void lcdOnVoltageRegulatorDisabled(const struct Event *event)
{
	TRISAbits.TRISA2 = 0;
	ANSELAbits.ANSA2 = 0;
}

void lcdDisable(void)
{
	if (lcdState.enableCount == 0)
		return;

	if (--lcdState.enableCount == 0)
	{
		lcdState.flags.isBusy = 1;
		ANSELAbits.ANSA2 = 1;
		TRISAbits.TRISA2 = 1;
		eventPublish(LCD_DISABLED, &eventEmptyArgs);
	}

	pwmTimerDisable();
	voltageRegulatorDisable();
}
