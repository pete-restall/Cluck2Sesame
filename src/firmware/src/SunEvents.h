#ifndef __CLUCK2SESAME_SRC_SUNEVENTS_H
#define __CLUCK2SESAME_SRC_SUNEVENTS_H
#include "Platform/Event.h"
#include "Platform/Clock.h"

#define SUN_EVENTS_CHANGED ((EventType) 0x50)
struct SunEventsChanged
{
	struct Time sunrise;
	struct Time sunset;
};

extern void sunEventsInitialise(void);

#endif
