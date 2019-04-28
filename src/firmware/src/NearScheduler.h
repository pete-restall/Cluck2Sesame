#ifndef __CLUCK3SESAME_SRC_NEARSCHEDULER_H
#define __CLUCK3SESAME_SRC_NEARSCHEDULER_H
#include <stdint.h>
#include "Event.h"

#define MS_TO_TICKS(x) ((uint8_t) (((x) + 9) / 10))

struct NearSchedule
{
	uint8_t ticks;
	EventType eventType;
	const void *eventArgs;
};

extern void nearSchedulerInitialise(void);
extern void nearSchedulerAdd(const struct NearSchedule *const schedule);

#endif
