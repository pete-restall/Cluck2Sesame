#include <xc.h>

#include "../Event.h"
#include "../VoltageRegulator.h"

#include "Motor.h"

static void onVoltageRegulatorEnabled(const struct Event *event);
static void onVoltageRegulatorDisabled(const struct Event *event);

void motorInitialise(void)
{
	LATC &= 0b00110011;
	ANSELC &= 0b00110011;
	TRISC &= 0b00110011;

	LATBbits.LATB1 = 0;
	ANSELBbits.ANSB1 = 1;
	TRISBbits.TRISB1 = 0;

	static const struct EventSubscription onVoltageRegulatorEnabledSubscription =
	{
		.type = VOLTAGE_REGULATOR_ENABLED,
		.handler = &onVoltageRegulatorEnabled,
		.state = (void *) 0
	};

	eventSubscribe(&onVoltageRegulatorEnabledSubscription);

	static const struct EventSubscription onVoltageRegulatorDisabledSubscription =
	{
		.type = VOLTAGE_REGULATOR_DISABLED,
		.handler = &onVoltageRegulatorDisabled,
		.state = (void *) 0
	};

	eventSubscribe(&onVoltageRegulatorDisabledSubscription);
}

static void onVoltageRegulatorEnabled(const struct Event *event)
{
	TRISBbits.TRISB1 = 1;
	TRISCbits.TRISC2 = 1;
	TRISCbits.TRISC3 = 1;
}

static void onVoltageRegulatorDisabled(const struct Event *event)
{
	TRISBbits.TRISB1 = 0;
	TRISCbits.TRISC2 = 0;
	TRISCbits.TRISC3 = 0;
}
