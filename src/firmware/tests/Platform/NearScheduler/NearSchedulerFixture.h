#ifndef __CLUCK3SESAME_TESTS_PLATFORM_NEARSCHEDULER_NEARSCHEDULERFIXTURE_H
#define __CLUCK3SESAME_TESTS_PLATFORM_NEARSCHEDULER_NEARSCHEDULERFIXTURE_H
#include <stdint.h>
#include "Platform/Event.h"
#include "Platform/PowerManagement.h"
#include "Platform/NearScheduler.h"

extern void tick(void);
extern void wokenFromSleep(void);
extern void assertNoHandlersCalled(void);
extern void assertHandlerCalledOnceWith(const void *state);
extern void assertHandlerCalledTimes(uint8_t times);
extern void assertHandlerCalledWith(const void *state);
extern void spyHandler(void *state);
extern void spyHandler2(void *state);
extern void dummyHandler(void *state);
extern void dummyHandler2(void *state);

extern const struct NearSchedule dummySchedule;
extern const struct NearSchedule dummySchedule2;

#endif
