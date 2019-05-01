#ifndef __CLUCK3SESAME_TESTS_NEARSCHEDULER_NEARSCHEDULERFIXTURE_H
#define __CLUCK3SESAME_TESTS_NEARSCHEDULER_NEARSCHEDULERFIXTURE_H
#include "Event.h"
#include "PowerManagement.h"
#include "NearScheduler.h"

extern void tick(void);
extern void wokenFromSleep(void);
extern void assertNoHandlersCalled(void);
extern void assertHandlerCalledOnceWith(const void *const state);
extern void assertHandlerCalledTimes(uint8_t times);
extern void assertHandlerCalledWith(const void *const state);
extern void spyHandler(const void *const state);
extern void dummyHandler(const void *const state);

extern const struct NearSchedule dummySchedule;

#endif
