#include <xc.h>
#include <stdint.h>

#include "Platform/Event.h"
#include "Platform/Temperature.h"
#include "Platform/Battery.h"

#include "BatteryFixture.h"

#include "../../NonDeterminism.h"

static uint16_t stubbedDiaFvra2xMillivolts;

const struct Event eventEmptyArgs = { };

volatile uint8_t isChargerGoodPinHigh;

void batteryFixtureSetUp(void)
{
	eventInitialise();
	stubbedDiaFvra2xMillivolts = FVR_IDEAL_MV;
	isChargerGoodPinHigh = 1;
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

void stubChargerGoodPinHigh(void)
{
	isChargerGoodPinHigh = 1;
}

void stubChargerGoodPinLow(void)
{
	isChargerGoodPinHigh = 0;
}

void stubTemperatureWithinChargingRange(void)
{
	static struct TemperatureSampled eventArgs;
	eventArgs.sample = anyWord();
	eventArgs.currentCelsius = (int16_t) (50 + anyWordLessThan(251));
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
