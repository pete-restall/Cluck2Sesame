#include <xc.h>
#include <stdint.h>

#include "Event.h"
#include "Nvm.h"
#include "PeriodicMonitor.h"

#include "Battery.h"

#define MINIMUM_FVR_ADC_COUNT (8 * 0x015a)
#define MAXIMUM_FVR_ADC_COUNT (8 * 0x0342)

static void onMonitoredParametersSampled(const struct Event *event);

static uint32_t scaledFvrNumerator;

void batteryInitialise(void)
{
	ANSELB &= ~(_ANSELB_ANSB3_MASK | _ANSELB_ANSB4_MASK | _ANSELB_ANSB5_MASK);
	LATBbits.LATB3 = 0;
	TRISBbits.TRISB3 = 0;
	TRISBbits.TRISB4 = 1;
	TRISBbits.TRISB5 = 1;
	INLVLBbits.INLVLB4 = 0;
	INLVLBbits.INLVLB5 = 0;

	static const struct EventSubscription onMonitoredParametersSampledSubscription =
	{
		.type = MONITORED_PARAMETERS_SAMPLED,
		.handler = &onMonitoredParametersSampled,
		.state = (void *) 0
	};

	eventSubscribe(&onMonitoredParametersSampledSubscription);

	scaledFvrNumerator = (uint32_t) 8192 * nvmWordAt(DIA_FVRA2X);
}

static void onMonitoredParametersSampled(const struct Event *event)
{
	const struct MonitoredParametersSampled *args = (const struct MonitoredParametersSampled *) event->args;
	if (args->flags.isVddRegulated || args->fvr < MINIMUM_FVR_ADC_COUNT || args->fvr > MAXIMUM_FVR_ADC_COUNT)
		return;

	static struct BatteryVoltageSampled eventArgs;
	eventArgs.sample = args->fvr;
	eventArgs.millivolts = (uint16_t) (scaledFvrNumerator / eventArgs.sample);
	eventPublish(BATTERY_VOLTAGE_SAMPLED, &eventArgs);
}

// TODO: INTERRUPT ON CHANGE

/*
TODO:
CHARGE BETWEEN 5degC AND 30degC.  CHARGE TO 3.9V AND DISCHARGE TO 3.0V
RB5 = /CHARGER_GOOD
RB4 = /CHARGING
RB3 = CHARGER_EN

onIoc
	- if /CHARGER_GOOD = 1 then CHARGER_EN = 0
	- if /CHARGING = 1 then CHARGER_EN = 0

onTemperatureSampled
	- if temperature < 5degC then CHARGER_EN = 0
	- if temperature > 30degC then CHARGER_EN = 0

onVbattSampled
	- if Vbatt >= 3.9V then CHARGER_EN = 0, return
	- if temperature < 5degC then CHARGER_EN = 0, return
	- if temperature > 30degC then CHARGER_EN = 0, return
	- if Vbatt < 3.1V then CHARGER_EN = 1
	- add or update (far) schedule for 13 minutes hence to set CHARGER_EN = 0

starting charging should publish an event
wanting to charge but unable to do so (ie. no power) should publish an event (ie. to enable a screen icon)
termination of charging should publish an event (with reason, ie. charged, power lost, etc.)
low voltage (<= 2.8V) should publish an event, which should enter a 'safety' mode (to be decided; probably involves disabling motor and waiting for charger to be connected)

determine lowest voltage that motor will start with; beyond certain level the motor should not be operated
characterise the motor - current over voltage range, current over temperature
*/
