#ifndef __CLUCK3SESAME_TESTS_DOOR_DOORFIXTURE_H
#define __CLUCK3SESAME_TESTS_DOOR_DOORFIXTURE_H
#include "Platform/FarScheduler.h"

extern void doorFixtureInitialise(void);
extern void doorFixtureShutdown(void);

extern void stubNvmSettingsForTimeDrivenMode(void);
extern void publishDateChanged(void);
extern void assertFarSchedulesAreEqualWithAnyNonNullArgs(
	const struct FarSchedule *const expected,
	const struct FarSchedule *const actual);

extern void stubNvmSettingsForManuallyDrivenMode(void);
extern void stubNvmSettingsForSunEventDrivenMode(void);
extern void stubNvmSettingsForUnspecifiedMode(void);

extern uint8_t farSchedulerAddCalls;
extern const struct FarSchedule *farSchedulerAddArgs[8];

#endif
