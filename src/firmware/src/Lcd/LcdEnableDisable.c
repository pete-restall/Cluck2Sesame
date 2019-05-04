#include <xc.h>
#include <stdint.h>

#include "../VoltageRegulator.h"
#include "../PwmTimer.h"

#include "Lcd.h"

void lcdEnable(void)
{
	voltageRegulatorEnable();
	pwmTimerEnable();
}

void lcdDisable(void)
{
	pwmTimerDisable();
	voltageRegulatorDisable();
}
