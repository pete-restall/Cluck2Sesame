#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/Temperature.h"
#include "Platform/Battery.h"

#include "BatteryFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

static void onBatteryChargerEnabled(const struct Event *event);
static void onBatteryChargerDisabled(const struct Event *event);

static uint16_t stubbedDiaFvra2xMillivolts;

const struct Event eventEmptyArgs = { };

const struct BatteryChargerEnabled *batteryChargerEnabledEventArgs;
const struct BatteryChargerDisabled *batteryChargerDisabledEventArgs;

volatile uint8_t isChargerGoodPinHigh;

void batteryFixtureSetUp(void)
{
	eventInitialise();
	stubbedDiaFvra2xMillivolts = FVR_IDEAL_MV;
	isChargerGoodPinHigh = 1;

	static const struct EventSubscription onBatteryChargerEnabledSubscription =
	{
		.type = BATTERY_CHARGER_ENABLED,
		.handler = &onBatteryChargerEnabled,
		.state = (void *) 0
	};

	eventSubscribe(&onBatteryChargerEnabledSubscription);

	static const struct EventSubscription onBatteryChargerDisabledSubscription =
	{
		.type = BATTERY_CHARGER_DISABLED,
		.handler = &onBatteryChargerDisabled,
		.state = (void *) 0
	};

	eventSubscribe(&onBatteryChargerDisabledSubscription);

	batteryChargerEnabledEventArgs = (const struct BatteryChargerEnabled *) 0;
	batteryChargerDisabledEventArgs = (const struct BatteryChargerDisabled *) 0;
}

static void onBatteryChargerEnabled(const struct Event *event)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(event, "Null event !");
	batteryChargerEnabledEventArgs = (const struct BatteryChargerEnabled *) event->args;
	TEST_ASSERT_NOT_NULL_MESSAGE(batteryChargerEnabledEventArgs, "Null event args !");
}

static void onBatteryChargerDisabled(const struct Event *event)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(event, "Null event !");
	batteryChargerDisabledEventArgs = (const struct BatteryChargerDisabled *) event->args;
	TEST_ASSERT_NOT_NULL_MESSAGE(batteryChargerDisabledEventArgs, "Null event args !");
}

void batteryFixtureTearDown(void)
{
}

uint16_t stubAnyDiaFvra2xMillivolts(void)
{
	uint16_t fvrMillivolts = FVR_IDEAL_MV - FVR_TOLERANCE_MV + anyWordLessThan(2 * FVR_TOLERANCE_MV + 1);
	stubDiaFvra2xMillivolts(fvrMillivolts);
	return fvrMillivolts;
}

void stubDiaFvra2xMillivolts(uint16_t millivolts)
{
	stubbedDiaFvra2xMillivolts = millivolts;
}

uint16_t nvmWordAt(uint16_t address)
{
	if (address == DIA_FVRA2X)
		return stubbedDiaFvra2xMillivolts;

	return 0;
}

void stubAllParametersThatWillEnableCharging(void)
{
	stubChargerGoodPinLow();
	stubTemperatureWithinChargingRange();
	stubBatteryVoltageWithinChargingRange();
	dispatchAllEvents();
}

void stubChargerGoodPinLow(void)
{
	isChargerGoodPinHigh = 0;
	publishWokenFromSleep();
}

void stubChargerGoodPinHigh(void)
{
	isChargerGoodPinHigh = 1;
	publishWokenFromSleep();
}

void stubTemperatureWithinChargingRange(void)
{
	stubTemperatureOf((int16_t) (50 + anyWordLessThan(251)));
}

void stubTemperatureOf(int16_t celsius)
{
	static struct TemperatureSampled eventArgs;
	eventArgs.sample = anyWord();
	eventArgs.currentCelsius = celsius;
	eventArgs.deltaCelsius = (int16_t) anyWord();
	eventArgs.deltaSeconds = anyByte();
	eventPublish(TEMPERATURE_SAMPLED, &eventArgs);
}

void stubBatteryVoltageWithinChargingRange(void)
{
	static struct BatteryVoltageSampled eventArgs;
	eventArgs.sample = anyWord();
	eventArgs.millivolts = anyWordLessThan(3100);
	eventPublish(BATTERY_VOLTAGE_SAMPLED, &eventArgs);
}
