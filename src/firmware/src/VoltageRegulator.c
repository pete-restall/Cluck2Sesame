#include <xc.h>

#include "Event.h"
#include "VoltageRegulator.h"

static uint8_t enableCount;

void voltageRegulatorInitialise(void)
{
	ANSELBbits.ANSB2 = 0;
	LATBbits.LATB2 = 0;
	TRISBbits.TRISB2 = 0;
	enableCount = 0;
}

void voltageRegulatorEnable(void)
{
	LATBbits.LATB2 = 1;
	enableCount++;
	// TODO: WE NEED TO PUBLISH VOLTAGE_REGULATOR_ENABLED, BUT ONLY ON THE FIRST TIME, AND ONLY AFTER A DELAY TO ALLOW THE LINE TO STABILISE.  SO THIS INVOLVES GETTING A TIMER MODULE WORKING FIRST
}

void voltageRegulatorDisable(void)
{
	if (enableCount > 1)
	{
		enableCount--;
		return;
	}

	if (enableCount == 1)
	{
		static const struct VoltageRegulatorDisabled eventArgs = { };
		eventPublish(VOLTAGE_REGULATOR_DISABLED, &eventArgs);
	}

	LATBbits.LATB2 = 0;
	enableCount = 0;
}
