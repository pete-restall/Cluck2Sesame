#ifndef __CLUCK2SESAME_SRC_PLATFORM_FARSCHEDULER_H
#define __CLUCK2SESAME_SRC_PLATFORM_FARSCHEDULER_H
#include <stdint.h>
#include "Event.h"

struct FarSchedule
{
	struct
	{
		uint8_t hour;
		uint8_t minute;
	} time;

	EventType eventType;
	const void *eventArgs;
};

extern void farSchedulerInitialise(void);
extern void farSchedulerAdd(const struct FarSchedule *schedule);
extern void farSchedulerRemove(const struct FarSchedule *schedule);

#endif
