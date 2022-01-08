#ifndef __CLUCK2SESAME_SRC_PLATFORM_NEARSCHEDULER_H
#define __CLUCK2SESAME_SRC_PLATFORM_NEARSCHEDULER_H
#include <stdint.h>

#define MS_TO_TICKS(x) ((uint8_t) (((x) + 3) / 4))

typedef void (*NearScheduleHandler)(void *state);

struct NearSchedule
{
	uint8_t ticks;
	NearScheduleHandler handler;
	void *state;
};

extern void nearSchedulerInitialise(void);
extern void nearSchedulerAdd(const struct NearSchedule *schedule);
extern void nearSchedulerAddOrUpdate(const struct NearSchedule *schedule);

#endif
