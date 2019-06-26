#ifndef __CLUCK3SESAME_TESTS_DOOR_DOORFIXTURE_H
#define __CLUCK3SESAME_TESTS_DOOR_DOORFIXTURE_H
#include "Platform/FarScheduler.h"
#include "SunEvents.h"

extern void doorFixtureInitialise(void);
extern void doorFixtureShutdown(void);

extern void stubNvmSettingsForTimeDrivenMode(void);
extern void stubNvmSettingsForManuallyDrivenMode(void);
extern void stubNvmSettingsForSunEventDrivenMode(void);
extern void stubNvmSettingsForSunEventDrivenModeWithOffsets(
	int8_t sunriseMinutes,
	int8_t sunsetMinutes);

extern void stubNvmSettingsForUnspecifiedMode(void);
extern void stubAnySunEvents(struct SunEventsChanged *eventArgs);
extern void stubDoorWithState(
	enum DoorState state,
	enum DoorTransition transition);

extern void stubMotorIsEnabled(void);
extern void stubMotorIsDisabled(void);

extern void publishDateChanged(void);
extern void publishNvmSettingsChanged(void);
extern void publishSunEventsChanged(
	const struct SunEventsChanged *eventArgs);

extern void publishDoorOpenScheduleActioned(void);
extern void publishDoorCloseScheduleActioned(void);
extern void publishDoorAbortedWithAnyFault(void);

extern void assertFarSchedulesAreEqualWithAnyNonNullArgs(
	const struct FarSchedule *expected,
	const struct FarSchedule *actual);

extern uint8_t farSchedulerAddCalls;
extern uint8_t farSchedulerAddSequence[8];
extern const struct FarSchedule *farSchedulerAddArgs[8];

extern uint8_t farSchedulerRemoveCalls;
extern uint8_t farSchedulerRemoveSequence[8];
extern const struct FarSchedule *farSchedulerRemoveArgs[8];

extern uint8_t motorEnableCalls;
extern uint8_t motorDisableCalls;

#endif
