#include <xc.h>
#include <stdint.h>
#include <stdlib.h>

#include "../Platform/Event.h"
#include "../Platform/Clock.h"
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
	div_t lookupIndex = div(
		(int) sunEventsCalculationContext.inputs.dayOfYear,
		LOOKUP_STEP);

	readLookupEntryInto(&lookupEntries[0], lookupIndex.quot);
	readLookupEntryInto(
		&lookupEntries[1],
		(lookupIndex.quot + 1) < LOOKUP_LENGTH ? lookupIndex.quot + 1 : 0);

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
	sunEventsCalculationContext.working.destination->hour =
		(uint8_t) hours.quot;

	sunEventsCalculationContext.working.destination->minute =
		(uint8_t) hours.rem;
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
