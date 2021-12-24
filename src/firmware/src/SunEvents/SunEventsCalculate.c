#include <xc.h>
#include <stdint.h>
#include <stdlib.h>

#include "../Platform/Event.h"
#include "../Platform/Nvm.h"
#include "../Platform/Clock.h"
#include "../Location.h"

#include "SunEvents.h"

#define MINUTES_PER_DEGREE_LONGITUDE 4

static void calculateSunEvent(void);
static void readLookupEntryInto(struct SunEventsLookupEntry *entry, uint8_t lookupIndex);

struct SunEventsCalculationContext sunEventsCalculationContext;
static struct SunEventsLookupEntry lookupEntries[2];

void sunEventsCalculate(void)
{
	static struct SunEventsChanged eventArgs;

	sunEventsCalculationContext.working.lookupPtr = (uint16_t) &sunriseLookupTable;
	sunEventsCalculationContext.working.destination = &eventArgs.sunrise;
	calculateSunEvent();

	sunEventsCalculationContext.working.lookupPtr = (uint16_t) &sunsetLookupTable;
	sunEventsCalculationContext.working.destination = &eventArgs.sunset;
	calculateSunEvent();

	eventPublish(SUN_EVENTS_CHANGED, &eventArgs);
}

static void calculateSunEvent(void)
{
	// TODO: REFACTOR THIS WHEN IT'S ALL KNOWN TO BE WORKING
	div_t lookupIndex = div(
		(int) sunEventsCalculationContext.inputs.dayOfYear,
		LOOKUP_STEP);

	uint8_t lookupIndexUint8 = (uint8_t) lookupIndex.quot;
	readLookupEntryInto(&lookupEntries[0], lookupIndexUint8);
	if (++lookupIndexUint8 >= LOOKUP_LENGTH)
		lookupIndexUint8 = 0;

	readLookupEntryInto(&lookupEntries[1], lookupIndexUint8);

	int dayDelta =
		lookupIndex.rem * (int)
		(lookupEntries[1].minuteOfDay - lookupEntries[0].minuteOfDay);

	div_t interpolatedMinuteOfDay = div(dayDelta, LOOKUP_STEP);
	if (interpolatedMinuteOfDay.rem >= LOOKUP_STEP / 2)
		interpolatedMinuteOfDay.quot++;

	interpolatedMinuteOfDay.quot += lookupEntries[0].minuteOfDay;

	div_t latitudeAdjustmentMinutes;
	if (sunEventsCalculationContext.inputs.latitudeOffset >= 0)
	{
		int offsetMinutes =
			lookupEntries[0].offsetMinutesNorth +
			(lookupEntries[1].offsetMinutesNorth - lookupEntries[0].offsetMinutesNorth) /
			LOOKUP_STEP;

		int interpolatedMinutes =
			offsetMinutes * sunEventsCalculationContext.inputs.latitudeOffset;

		latitudeAdjustmentMinutes = div(
			interpolatedMinutes,
			LOOKUP_LATITUDE_NORTH - LOOKUP_LATITUDE);
	}
	else
	{
		int offsetMinutes =
			lookupEntries[0].offsetMinutesSouth +
			(lookupEntries[1].offsetMinutesSouth - lookupEntries[0].offsetMinutesSouth) /
			LOOKUP_STEP;

		int interpolatedMinutes =
			offsetMinutes * -sunEventsCalculationContext.inputs.latitudeOffset;

		latitudeAdjustmentMinutes = div(
			interpolatedMinutes,
			LOOKUP_LATITUDE - LOOKUP_LATITUDE_SOUTH);
	}

	latitudeAdjustmentMinutes.quot +=
		latitudeAdjustmentMinutes.rem >= LONGLAT_HALF_DEGREE
			? 1
			: latitudeAdjustmentMinutes.rem <= -LONGLAT_HALF_DEGREE
				? -1
				: 0;

	int longitudeDelta =
		sunEventsCalculationContext.inputs.longitudeOffset *
		-MINUTES_PER_DEGREE_LONGITUDE;

	div_t longitudeAdjustmentMinutes = div(longitudeDelta, LONGLAT_ONE_DEGREE);
	longitudeAdjustmentMinutes.quot +=
		longitudeAdjustmentMinutes.rem >= LONGLAT_HALF_DEGREE
			? 1
			: longitudeAdjustmentMinutes.rem <= -LONGLAT_HALF_DEGREE
				? -1
				: 0;

	int minuteOfDay =
		interpolatedMinuteOfDay.quot +
		latitudeAdjustmentMinutes.quot +
		longitudeAdjustmentMinutes.quot;

	div_t hours = div(minuteOfDay, 60);
	sunEventsCalculationContext.working.destination->hour = (uint8_t) hours.quot;
	sunEventsCalculationContext.working.destination->minute = (uint8_t) hours.rem;
}

static void readLookupEntryInto(struct SunEventsLookupEntry *entry, uint8_t lookupIndex)
{
	uint16_t lookupPtr = (sunEventsCalculationContext.working.lookupPtr + (((uint16_t) lookupIndex) << 1)) & 0x7fff;
	uint16_t word0 = nvmWordAt(lookupPtr);
	uint16_t word1 = nvmWordAt(lookupPtr + 1);

	entry->minuteOfDay = (uint16_t) (((word0 >> 2) & 0b11111000000) | ((word1 >> 8) & 0b111111));
	entry->offsetMinutesNorth = (int8_t) (word0 & 0xff);
	entry->offsetMinutesSouth = (int8_t) (word1 & 0xff);
}
