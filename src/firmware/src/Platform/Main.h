#ifndef __CLUCK2SESAME_SRC_PLATFORM_MAIN_H
#define __CLUCK2SESAME_SRC_PLATFORM_MAIN_H
#include "Event.h"

#define SYSTEM_INITIALISED ((EventType) 0x01)
struct SystemInitialised { EMPTY_EVENT_ARGS };

#define ALL_EVENTS_DISPATCHED ((EventType) 0x02)
struct AllEventsDispatched { EMPTY_EVENT_ARGS };

extern void initialise(void);
extern void applicationInitialise(void);
extern void poll(void);

#endif
