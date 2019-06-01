#ifndef __CLUCK3SESAME_SRC_SUNEVENTS_SUNEVENTS_H
#define __CLUCK3SESAME_SRC_SUNEVENTS_SUNEVENTS_H
#include "../Platform/Clock.h"
#include "../SunEvents.h"
#include "../Location.h"

#define LOOKUP_STEP 6
#define LOOKUP_LENGTH (366 / LOOKUP_STEP)
#define LOOKUP_LATITUDE (55 * LONGLAT_ONE_DEGREE)
#define LOOKUP_LATITUDE_NORTH (LOOKUP_LATITUDE + 5 * LONGLAT_ONE_DEGREE)
#define LOOKUP_LATITUDE_SOUTH (LOOKUP_LATITUDE - 5 * LONGLAT_ONE_DEGREE)
#define LOOKUP_LONGITUDE (0 * LONGLAT_ONE_DEGREE)

struct SunEventsCalculationContext
{
	struct
	{
		uint16_t dayOfYear;
		int8_t latitudeOffset;
		int8_t longitudeOffset;
	} inputs;

	struct
	{
		int lookupPtr;
		struct Time *destination;
	} working;
};

struct SunEventsLookupEntry
{
	uint16_t minuteOfDay;
	int8_t offsetMinutesNorth;
	int8_t offsetMinutesSouth;
};

extern struct SunEventsCalculationContext sunEventsCalculationContext;

extern const uint8_t sunriseLookupTable[];
extern const uint8_t sunsetLookupTable[];

extern void sunEventsCalculate(void);

#endif
