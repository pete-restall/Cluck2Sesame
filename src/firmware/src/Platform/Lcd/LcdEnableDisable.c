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

	// TRISAbits.TRISA2 = 1; // TODO: NEEDS TESTING.  SAVES 6mA THOUGH !
}

// TODO: NEEDS TESTING.  SAVES 6mA THOUGH !
// TODO: THE IDEA IS TO TRI-STATE THE CONTRAST PIN WHEN THE VOLTAGE REGULATOR
// IS ENABLED BUT THE LCD IS NOT IN USE, THUS SAVING 6mA FROM NOT SWITCHING
// THE CONTRAST PWM.  THE PIN MUST BE AN OUTPUT WHEN THE VOLTAGE REGULATOR IS
// DISABLED, HOWEVER, SINCE OTHERWISE IT WOULD FLOAT (THE LATCH IS INITIALISED
// TO ZERO AND SHOULD NOT BE SET ANYWHERE ELSE, SO ALWAYS PULLED LOW SO WON'T
// TRY TO SOURCE CURRENT THROUGH THE LCD'S ESD DIODES)
//
//void onVoltageRegulatorDisabledOrLcdEnabled(const struct Event *event)
//{
//	THESE EVENTS NEED TO BE SUBSCRIBED TO, PROBABLY IN THE LCD INITIALISE.
//	TRISAbits.TRISA2 = 0;
//}

void lcdDisable(void)
{
	if (lcdState.enableCount == 0)
		return;

	if (--lcdState.enableCount == 0)
		lcdState.flags.isBusy = 1;
		// TRISAbits.TRISA2 = 1; // TODO: NEEDS TESTING.  SAVES 6mA THOUGH !

	pwmTimerDisable();
	voltageRegulatorDisable();
}
