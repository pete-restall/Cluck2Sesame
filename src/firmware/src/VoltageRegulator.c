#include <xc.h>

#include "Event.h"
#include "NearScheduler.h"
#include "VoltageRegulator.h"

static void onVoltageRailStabilised(void *state);

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

	// TODO: PUBLISH VOLTAGE_REGULATOR_ENABLED AFTER ABOUT 64ms, *IF* IT HAS NOT BEEN DISABLED IN THE MEANTIME.  IF THE VOLTAGE REGULATOR HAS ALREADY BEEN ENABLED THEN DO NOT PUBLISH ANOTHER VOLTAGE_REGULATOR_ENABLED.
	static const struct NearSchedule waitForVoltageRailToStabilise =
	{
		.ticks = MS_TO_TICKS(64),
		.handler = onVoltageRailStabilised
	};

	nearSchedulerAdd(&waitForVoltageRailToStabilise);
}

static void onVoltageRailStabilised(void *state)
{
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
