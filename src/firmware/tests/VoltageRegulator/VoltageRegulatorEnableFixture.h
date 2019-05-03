#ifndef __CLUCK3SESAME_TESTS_VOLTAGEREGULATOR_VOLTAGEREGULATORENABLEFIXTURE_H
#define __CLUCK3SESAME_TESTS_VOLTAGEREGULATOR_VOLTAGEREGULATORENABLEFIXTURE_H
#include "NearScheduler.h"

extern void assertEventPublishNotCalled(
	EventType type,
	const void *const args,
	int numCalls);

extern void assertScheduleAddedWithHandler(void);

extern const struct NearSchedule *requestedSchedule;

#endif
