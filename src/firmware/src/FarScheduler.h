#ifndef __CLUCK3SESAME_SRC_FARSCHEDULER_H
#define __CLUCK3SESAME_SRC_FARSCHEDULER_H
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
extern void farSchedulerAdd(const struct FarSchedule *const schedule);

#endif
