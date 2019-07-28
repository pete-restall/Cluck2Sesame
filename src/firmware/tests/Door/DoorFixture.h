#ifndef __CLUCK3SESAME_TESTS_DOOR_DOORFIXTURE_H
#define __CLUCK3SESAME_TESTS_DOOR_DOORFIXTURE_H
#include "Platform/FarScheduler.h"
#include "Platform/Motor.h"
#include "SunEvents.h"

#define PULSES_PER_10CM 1576
#define PULSES_PER_1M 15756
#define PULSES_PER_2MM 3

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
extern void publishMotorStopped(const struct MotorStopped *eventArgs);
extern void publishMotorStoppedWithNoFaults(void);
extern void publishMotorStoppedWithNoFaultsOnRaising(void);
extern void publishMotorStoppedWithNoFaultsOnLowering(void);
extern void publishMotorStoppedWithFaults(void);
extern void publishMotorStoppedWithFaultsOnRaising(void);
extern void publishMotorStoppedWithFaultsOnLowering(void);
extern void publishMotorStoppedWithNonCurrentLimitFaultOnRaising(void);

extern void assertFarSchedulesAreEqualWithAnyNonNullArgs(
	const struct FarSchedule *expected,
	const struct FarSchedule *actual);

extern void mockOnDoorAborted(void);
extern void mockOnDoorOpened(void);
extern void mockOnDoorClosed(void);

extern void enterFindBottomState(void);

extern uint8_t anyUnknownMotorFault(void);

extern uint8_t farSchedulerAddCalls;
extern uint8_t farSchedulerAddSequence[8];
extern const struct FarSchedule *farSchedulerAddArgs[8];

extern uint8_t farSchedulerRemoveCalls;
extern uint8_t farSchedulerRemoveSequence[8];
extern const struct FarSchedule *farSchedulerRemoveArgs[8];

extern uint8_t onDoorAbortedCalls;
extern const struct DoorAborted *onDoorAbortedArgs[8];

extern uint8_t onDoorOpenedCalls;
extern const struct DoorOpened *onDoorOpenedArgs[8];

extern uint8_t onDoorClosedCalls;
extern const struct DoorClosed *onDoorClosedArgs[8];

extern uint8_t motorEnableCalls;
extern uint8_t motorEnableSequence;
extern uint8_t motorDisableCalls;
extern uint8_t motorDisableSequence;
extern uint8_t motorOnCalls;
extern uint8_t motorOnSequence;
extern int16_t motorOnArgs[8];
extern uint8_t motorOffCalls;
extern uint8_t motorOffSequence;
extern uint8_t motorLimitIsNoLoadCalls;
extern uint8_t motorLimitIsNoLoadSequence;
extern uint8_t motorLimitIsMaximumLoadCalls;
extern uint8_t motorLimitIsMaximumLoadSequence;

#endif
