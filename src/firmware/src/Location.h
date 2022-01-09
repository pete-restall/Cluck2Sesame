#ifndef __CLUCK2SESAME_SRC_LOCATION_H
#define __CLUCK2SESAME_SRC_LOCATION_H
#include <stdint.h>
#include "Platform/Event.h"

#define LONGLAT_RESOLUTION 10
#define LONGLAT_ONE_DEGREE (1 * LONGLAT_RESOLUTION)
#define LONGLAT_HALF_DEGREE (1 * LONGLAT_RESOLUTION / 2)

struct Location;

#define LOCATION_CHANGED ((EventType) 0x48)
struct LocationChanged
{
	const struct Location *location;
};

struct Location
{
	int8_t latitudeOffset;
	int8_t longitudeOffset;
};

extern void locationInitialise(void);

#endif
