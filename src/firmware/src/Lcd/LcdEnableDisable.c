#include <xc.h>
#include <stdint.h>

#include "../VoltageRegulator.h"
#include "../PwmTimer.h"

#include "Lcd.h"

uint8_t lcdEnableCount;

void lcdEnable(void)
{
	voltageRegulatorEnable();
	pwmTimerEnable();
	if (++lcdEnableCount == 1 && voltageRegulatorIsEnabled())
		lcdConfigure();
}

void onVoltageRegulatorEnabled(const struct Event *event)
{
	if (lcdEnableCount != 0)
		lcdConfigure();
}

void lcdDisable(void)
{
	if (lcdEnableCount == 0)
		return;

	--lcdEnableCount;
	pwmTimerDisable();
	voltageRegulatorDisable();
}
