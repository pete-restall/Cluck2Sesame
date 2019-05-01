#ifndef __CLUCK3SESAME_SRC_NEARSCHEDULER_H
#define __CLUCK3SESAME_SRC_NEARSCHEDULER_H
#include <stdint.h>

#define MS_TO_TICKS(x) ((uint8_t) (((x) + 3) / 4))

struct NearSchedule;
typedef void (*NearScheduleHandler)(const struct NearSchedule *schedule);

struct NearSchedule
{
	uint8_t ticks;
	NearScheduleHandler handler;
	void *state;
};

extern void nearSchedulerInitialise(void);
extern void nearSchedulerAdd(const struct NearSchedule *const schedule);

#endif
