#ifndef __CLUCK3SESAME_SRC_CLOCK_H
#define __CLUCK3SESAME_SRC_CLOCK_H
#include "Event.h"

#define DATE_CHANGED ((EventType) 0x10)
struct DateChanged { EMPTY_EVENT_ARGS }; // TODO !

extern void clockInitialise(void);

#endif
