#ifndef __CLUCK3SESAME_SRC_MAIN_H
#define __CLUCK3SESAME_SRC_MAIN_H
#include "Event.h"

#define SYSTEM_INITIALISED ((EventType) 0x01)

struct SystemInitialised { EMPTY_EVENT_ARGS };

extern void initialise(void);
extern void poll(void);

#endif
