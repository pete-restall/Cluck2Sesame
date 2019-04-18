#ifndef __CLUCK3SESAME_SRC_SUNEVENTS_SUNEVENTS_H
#define __CLUCK3SESAME_SRC_SUNEVENTS_SUNEVENTS_H
#include "../SunEvents.h"
#include "../Clock.h"

#define LOOKUP_STEP 6
#define LOOKUP_LENGTH (366 / LOOKUP_STEP)

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
	int8_t offsetNorth;
	int8_t offsetSouth;
};

extern struct SunEventsCalculationContext sunEventsCalculationContext;

extern const uint8_t sunriseLookupTable[];
extern const uint8_t sunsetLookupTable[];

extern void sunEventsCalculate(void);

#endif
