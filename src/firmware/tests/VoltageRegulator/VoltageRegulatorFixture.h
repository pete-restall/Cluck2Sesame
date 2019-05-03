#ifndef __CLUCK3SESAME_TESTS_VOLTAGEREGULATOR_VOLTAGEREGULATORFIXTURE_H
#define __CLUCK3SESAME_TESTS_VOLTAGEREGULATOR_VOLTAGEREGULATORFIXTURE_H
#include "NearScheduler.h"

extern void assertEventPublishNotCalled(
	EventType type,
	const void *const args,
	int numCalls);

extern void assertScheduleAddedWithHandler(void);
extern void fullyEnableVoltageRegulator(void);
extern void callScheduleHandlerAndForget(void);
extern void callScheduleHandlerIfPresentAndForget(void);

extern const struct NearSchedule *requestedSchedule;

#endif
