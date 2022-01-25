#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "Event.h"
#include "Nvm.h"
#include "PowerManagement.h"
#include "PeriodicMonitor.h"
#include "Temperature.h"

#include "Battery.h"

#define MINIMUM_FVR_ADC_COUNT (8 * 0x015a)
#define MAXIMUM_FVR_ADC_COUNT (8 * 0x0342)

#define MINIMUM_BATTERY_MILLIVOLTS 2500
#define BATTERY_START_CHARGING_MILLIVOLTS 3850
#define BATTERY_STOP_CHARGING_MILLIVOLTS 3930

static void onMonitoredParametersSampled(const struct Event *event);
static void onWokenFromSleep(const struct Event *event);
static void evaluateChargingConditions(void);
static void onTemperatureSampled(const struct Event *event);

static uint32_t scaledFvrNumerator;
static bool isChargerGood;
static bool isTemperatureWithinChargingRange;
static uint16_t batteryVoltageMillivolts;
static uint8_t batteryVoltageSampledSafetyCount;

void batteryInitialise(void)
{
	ANSELB &= ~(_ANSELB_ANSB3_MASK | _ANSELB_ANSB4_MASK | _ANSELB_ANSB5_MASK);
	LATBbits.LATB3 = 0;
	TRISBbits.TRISB3 = 0;
	TRISBbits.TRISB4 = 1;
	TRISBbits.TRISB5 = 1;
	INLVLBbits.INLVLB4 = 0;
	INLVLBbits.INLVLB5 = 0;

	PMD0bits.IOCMD = 0;
	IOCBPbits.IOCBP5 = 1;
	IOCBNbits.IOCBN5 = 1;
	PIE0bits.IOCIE = 1;

	static const struct EventSubscription onMonitoredParametersSampledSubscription =
	{
		.type = MONITORED_PARAMETERS_SAMPLED,
		.handler = &onMonitoredParametersSampled,
		.state = (void *) 0
	};

	eventSubscribe(&onMonitoredParametersSampledSubscription);

	static const struct EventSubscription onWokenFromSleepSubscription =
	{
		.type = WOKEN_FROM_SLEEP,
		.handler = &onWokenFromSleep,
		.state = (void *) 0
	};

	eventSubscribe(&onWokenFromSleepSubscription);

	static const struct EventSubscription onTemperatureSampledSubscription =
	{
		.type = TEMPERATURE_SAMPLED,
		.handler = &onTemperatureSampled,
		.state = (void *) 0
	};

	eventSubscribe(&onTemperatureSampledSubscription);

	scaledFvrNumerator = (uint32_t) 8192 * nvmWordAt(DIA_FVRA2X);
	isChargerGood = false;
	isTemperatureWithinChargingRange = false;
	batteryVoltageMillivolts = 0;
	batteryVoltageSampledSafetyCount = 0;
}

static void onMonitoredParametersSampled(const struct Event *event)
{
	const struct MonitoredParametersSampled *args = (const struct MonitoredParametersSampled *) event->args;
	if (!args->flags.isVddRegulated && args->fvr >= MINIMUM_FVR_ADC_COUNT && args->fvr <= MAXIMUM_FVR_ADC_COUNT)
	{
		static struct BatteryVoltageSampled eventArgs;
		eventArgs.sample = args->fvr;
		eventArgs.millivolts = (uint16_t) (scaledFvrNumerator / eventArgs.sample);
		eventPublish(BATTERY_VOLTAGE_SAMPLED, &eventArgs);

		batteryVoltageMillivolts = eventArgs.millivolts;
		batteryVoltageSampledSafetyCount = 0;
	}
	else if (batteryVoltageSampledSafetyCount < 0xff)
		batteryVoltageSampledSafetyCount++;

	evaluateChargingConditions();
}

static void evaluateChargingConditions(void)
{
	bool isBatteryVoltageWithinChargingRange =
		batteryVoltageSampledSafetyCount < 3 &&
		batteryVoltageMillivolts >= MINIMUM_BATTERY_MILLIVOLTS && (
			(LATBbits.LATB3 && batteryVoltageMillivolts < BATTERY_STOP_CHARGING_MILLIVOLTS) ||
			(!LATBbits.LATB3 && batteryVoltageMillivolts < BATTERY_START_CHARGING_MILLIVOLTS));

	if (isChargerGood && isTemperatureWithinChargingRange && isBatteryVoltageWithinChargingRange)
	{
		if (LATBbits.LATB3 == 0)
		{
			LATBbits.LATB3 = 1;
			eventPublish(BATTERY_CHARGER_ENABLED, &eventEmptyArgs);
		}
	}
	else
	{
		if (LATBbits.LATB3 != 0)
		{
			LATBbits.LATB3 = 0;
			eventPublish(BATTERY_CHARGER_DISABLED, &eventEmptyArgs);
		}
	}
}

static void onWokenFromSleep(const struct Event *event)
{
	IOCBFbits.IOCBF5 = 0;
	isChargerGood = !PORTBbits.RB5;
	evaluateChargingConditions();
}

static void onTemperatureSampled(const struct Event *event)
{
	const struct TemperatureSampled *args = (const struct TemperatureSampled *) event->args;
	isTemperatureWithinChargingRange = args->currentCelsius >= CELSIUS(5) && args->currentCelsius <= CELSIUS(30);
	evaluateChargingConditions();
}
