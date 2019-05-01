#ifndef __CLUCK3SESAME_TESTS_NEARSCHEDULER_NEARSCHEDULERFIXTURE_H
#define __CLUCK3SESAME_TESTS_NEARSCHEDULER_NEARSCHEDULERFIXTURE_H
#include "Event.h"
#include "PowerManagement.h"
#include "NearScheduler.h"

extern void tick(void);
extern void wokenFromSleep(void);
extern void assertNoHandlersCalled(void);
extern void assertHandlerCalledOnceWith(const struct NearSchedule *const schedule);
extern void assertHandlerCalledTimes(uint8_t times);
extern void assertHandlerCalledWith(const struct NearSchedule *const schedule);
extern void spyHandler(const struct NearSchedule *const schedule);
extern void dummyHandler(const struct NearSchedule *const schedule);

extern const struct NearSchedule dummySchedule;

#endif
