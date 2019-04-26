#include <xc.h>
#include <stdint.h>

#include "../Event.h"
#include "../Clock.h"
#include "../Location.h"

#include "SunEvents.h"

#define MINUTES_PER_DEGREE_LONGITUDE 4

static void calculateSunEvent(void);

static void readLookupEntryInto(
	struct SunEventsLookupEntry *const entry,
	int lookupIndex);

static void readFlashWordInto(uint16_t *const destination, int ptr);

struct SunEventsCalculationContext sunEventsCalculationContext;
static struct SunEventsLookupEntry lookupEntries[2];

void sunEventsCalculate(void)
{
	static struct SunEventsChanged eventArgs;

	sunEventsCalculationContext.working.lookupPtr = (int) sunriseLookupTable;
	sunEventsCalculationContext.working.destination = &eventArgs.sunrise;
	calculateSunEvent();

	sunEventsCalculationContext.working.lookupPtr = (int) sunsetLookupTable;
	sunEventsCalculationContext.working.destination = &eventArgs.sunset;
	calculateSunEvent();

	eventPublish(SUN_EVENTS_CHANGED, &eventArgs);
}

static void calculateSunEvent(void)
{
	// TODO: REFACTOR THIS WHEN IT'S ALL KNOWN TO BE WORKING
	// TODO: LOOK AT USING 'div()' FROM stdlib AS A SPACE SAVER ?
	int16_t lookupIndex =
		sunEventsCalculationContext.inputs.dayOfYear / LOOKUP_STEP;

	int16_t lookupResidual =
		sunEventsCalculationContext.inputs.dayOfYear % LOOKUP_STEP;

	readLookupEntryInto(&lookupEntries[0], lookupIndex);
	readLookupEntryInto(
		&lookupEntries[1],
		(lookupIndex + 1) < LOOKUP_LENGTH ? lookupIndex + 1 : 0);

	int16_t dayDelta =
		lookupResidual * (int)
		(lookupEntries[1].minuteOfDay - lookupEntries[0].minuteOfDay);

	int16_t interpolatedMinuteOfDay = dayDelta / LOOKUP_STEP;
	int16_t interpolatedMinuteOfDayResidual = dayDelta % LOOKUP_STEP;
	if (interpolatedMinuteOfDayResidual >= LOOKUP_STEP / 2)
		interpolatedMinuteOfDay++;

	interpolatedMinuteOfDay += lookupEntries[0].minuteOfDay;

	int16_t latitudeAdjustmentMinutes;
	if (sunEventsCalculationContext.inputs.latitudeOffset >= 0)
	{
		int16_t offsetMinutes = lookupEntries[0].offsetMinutesNorth + (lookupEntries[1].offsetMinutesNorth - lookupEntries[0].offsetMinutesNorth) / LOOKUP_STEP;
		int16_t interpolatedMinutes = offsetMinutes * sunEventsCalculationContext.inputs.latitudeOffset;
		latitudeAdjustmentMinutes = interpolatedMinutes / (LOOKUP_LATITUDE_NORTH - LOOKUP_LATITUDE);
		int16_t residual = interpolatedMinutes % (LOOKUP_LATITUDE_NORTH - LOOKUP_LATITUDE);
		latitudeAdjustmentMinutes = latitudeAdjustmentMinutes + (residual >= LONGLAT_HALF_DEGREE ? 1 : residual <= -LONGLAT_HALF_DEGREE ? -1 : 0);
	}
	else
	{
		int16_t offsetMinutes = lookupEntries[0].offsetMinutesSouth + (lookupEntries[1].offsetMinutesSouth - lookupEntries[0].offsetMinutesSouth) / LOOKUP_STEP;
		int16_t interpolatedMinutes = offsetMinutes * -sunEventsCalculationContext.inputs.latitudeOffset;
		latitudeAdjustmentMinutes = interpolatedMinutes / (LOOKUP_LATITUDE - LOOKUP_LATITUDE_SOUTH);
		int16_t residual = interpolatedMinutes % (LOOKUP_LATITUDE - LOOKUP_LATITUDE_SOUTH);
		latitudeAdjustmentMinutes = latitudeAdjustmentMinutes + (residual >= LONGLAT_HALF_DEGREE ? 1 : residual <= -LONGLAT_HALF_DEGREE ? -1 : 0);
	}

	int16_t longitudeDelta =
		sunEventsCalculationContext.inputs.longitudeOffset *
		-MINUTES_PER_DEGREE_LONGITUDE;

	int16_t longitudeMinutes = longitudeDelta / LONGLAT_ONE_DEGREE;
	int16_t longitudeMinutesResidual = longitudeDelta % LONGLAT_ONE_DEGREE;
	int16_t longitudeAdjustmentMinutes = longitudeMinutes + (longitudeMinutesResidual >= LONGLAT_HALF_DEGREE ? 1 : longitudeMinutesResidual <= -LONGLAT_HALF_DEGREE ? -1 : 0);

	uint16_t minuteOfDay = interpolatedMinuteOfDay + latitudeAdjustmentMinutes + longitudeAdjustmentMinutes;

	sunEventsCalculationContext.working.destination->hour =
		minuteOfDay / 60;

	sunEventsCalculationContext.working.destination->minute =
		minuteOfDay % 60;
}

static void readLookupEntryInto(
	struct SunEventsLookupEntry *const entry,
	int lookupIndex)
{
	int lookupPtr =
		sunEventsCalculationContext.working.lookupPtr + (lookupIndex << 1);

	uint16_t word0, word1;
	readFlashWordInto(&word0, lookupPtr);
	readFlashWordInto(&word1, lookupPtr + 1);

	entry->minuteOfDay = (uint16_t) (
		((word0 >> 2) & 0b11111000000) | ((word1 >> 8) & 0b111111));

	entry->offsetMinutesNorth = (uint8_t) (word0 & 0xff);
	entry->offsetMinutesSouth = (uint8_t) (word1 & 0xff);
}

// TODO: THIS CAN BE MOVED INTO SOMETHING COMMON, SINCE THE DAYLIGHT SAVINGS
// LOOKUP WILL ALSO REQUIRE WORD-BASED FLASH READING
static void readFlashWordInto(uint16_t *const destination, int ptr)
{
	NVMCON1bits.NVMREGS = 0;
	NVMADRH = (ptr >> 8) & 0xff;
	NVMADRL = (ptr >> 0) & 0xff;
	NVMCON1bits.RD = 1;

	*destination = (((uint16_t) NVMDATH) << 8) | NVMDATL;
}
