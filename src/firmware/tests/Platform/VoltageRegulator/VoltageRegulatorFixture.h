#ifndef __CLUCK3SESAME_TESTS_PLATFORM_VOLTAGEREGULATOR_VOLTAGEREGULATORFIXTURE_H
#define __CLUCK3SESAME_TESTS_PLATFORM_VOLTAGEREGULATOR_VOLTAGEREGULATORFIXTURE_H
#include "Platform/NearScheduler.h"

extern void assertEventPublishNotCalled(
	EventType type,
	const void *args,
	int numCalls);

extern void assertScheduleAddedWithHandler(void);
extern void fullyEnableVoltageRegulator(void);
extern void callScheduleHandlerAndForget(void);
extern void callScheduleHandlerIfPresentAndForget(void);
extern void fullyEnableVoltageRegulatorWithoutAssertions(void);

extern const struct NearSchedule *requestedSchedule;

#endif
