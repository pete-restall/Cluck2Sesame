#ifndef __CLUCK2SESAME_SRC_PLATFORM_POWERMANAGEMENT_H
#define __CLUCK2SESAME_SRC_PLATFORM_POWERMANAGEMENT_H
#include "Event.h"

#define WOKEN_FROM_SLEEP ((EventType) 0x08)
struct WokenFromSleep { EMPTY_EVENT_ARGS };

extern void powerManagementInitialise(void);

#endif
