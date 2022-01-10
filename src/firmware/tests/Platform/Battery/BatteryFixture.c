#include <xc.h>
#include <stdint.h>

#include "BatteryFixture.h"

#include "../../NonDeterminism.h"

static uint16_t stubbedDiaFvra2xMillivolts;

void batteryFixtureSetUp(void)
{
	stubbedDiaFvra2xMillivolts = FVR_IDEAL_MV;
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
