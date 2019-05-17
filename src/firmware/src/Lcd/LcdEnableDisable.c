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

void onVoltageRegulatorEnabled(const struct Event *event)
{
	if (lcdState.enableCount != 0)
		lcdConfigure();
}

void lcdDisable(void)
{
	if (lcdState.enableCount == 0)
		return;

	if (--lcdState.enableCount == 0)
		lcdState.flags.busy = 1;

	pwmTimerDisable();
	voltageRegulatorDisable();
}
