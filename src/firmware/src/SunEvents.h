#ifndef __CLUCK3SESAME_SRC_SUNEVENTS_H
#define __CLUCK3SESAME_SRC_SUNEVENTS_H
#include "Event.h"

#define SUN_EVENTS_CHANGED ((EventType) 0x18)
struct SunEventsChanged { EMPTY_EVENT_ARGS }; // TODO !

extern void sunEventsInitialise(void);

#endif
