#ifndef __CLUCK3SESAME_SRC_SUNEVENTS_H
#define __CLUCK3SESAME_SRC_SUNEVENTS_H
#include "Platform/Event.h"
#include "Platform/Clock.h"

#define SUN_EVENTS_CHANGED ((EventType) 0x48)
struct SunEventsChanged
{
	struct Time sunrise;
	struct Time sunset;
};

extern void sunEventsInitialise(void);

#endif
