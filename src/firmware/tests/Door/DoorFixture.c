#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/NvmSettings.h"
#include "Platform/Motor.h"
#include "ApplicationNvmSettings.h"
#include "SunEvents.h"
#include "Door/Door.h"
#include "Door.h"

#include "DoorFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"
#include "../NvmSettingsFixture.h"

static void onDoorAborted(const struct Event *event);
static void onDoorOpened(const struct Event *event);
static void onDoorClosed(const struct Event *event);

static uint8_t callSequence;

uint8_t farSchedulerAddCalls;
uint8_t farSchedulerAddSequence[8];
const struct FarSchedule *farSchedulerAddArgs[8];

uint8_t farSchedulerRemoveCalls;
uint8_t farSchedulerRemoveSequence[8];
const struct FarSchedule *farSchedulerRemoveArgs[8];

uint8_t onDoorAbortedCalls;
const struct DoorAborted *onDoorAbortedArgs[8];

uint8_t onDoorOpenedCalls;
const struct DoorOpened *onDoorOpenedArgs[8];

uint8_t onDoorClosedCalls;
const struct DoorClosed *onDoorClosedArgs[8];

uint8_t motorEnableCalls;
uint8_t motorEnableSequence;
uint8_t motorDisableCalls;
uint8_t motorDisableSequence;
uint8_t motorOnCalls;
uint8_t motorOnSequence;
int16_t motorOnArgs[8];
uint8_t motorOffCalls;
uint8_t motorOffSequence;
uint8_t motorLimitIsNoLoadCalls;
uint8_t motorLimitIsNoLoadSequence;
uint8_t motorLimitIsMaximumLoadCalls;
uint8_t motorLimitIsMaximumLoadSequence;

static uint8_t motorIsEnabledReturns;

void doorFixtureInitialise(void)
{
	callSequence = 0;
	farSchedulerAddCalls = 0;
	farSchedulerRemoveCalls = 0;
	onDoorAbortedCalls = 0;
	onDoorOpenedCalls = 0;
	onDoorClosedCalls = 0;
	motorIsEnabledReturns = 0;
	motorEnableCalls = 0;
	motorEnableSequence = 0;
	motorDisableCalls = 0;
	motorDisableSequence = 0;
	motorOnCalls = 0;
	motorOnSequence = 0;
	motorOffCalls = 0;
	motorOffSequence = 0;
	motorLimitIsNoLoadCalls = 0;
	motorLimitIsNoLoadSequence = 0;
	motorLimitIsMaximumLoadCalls = 0;
	motorLimitIsMaximumLoadSequence = 0;

	eventInitialise();
	doorInitialise();
}

void doorFixtureShutdown(void)
{
}

void stubNvmSettingsForTimeDrivenMode(void)
{
	union ApplicationNvmSettings settings =
	{
		.door =
		{
			.mode =
			{
				.isTimeDriven = 1,
				.isManuallyOverridden = 0,
				.isSunEventDriven = 0
			},
			.autoOpenTime =
			{
				.hour = anyByteLessThan(24),
				.minute = anyByteLessThan(60)
			},
			.autoCloseTime =
			{
				.hour = anyByteLessThan(24),
				.minute = anyByteLessThan(60)
			}
		}
	};

	stubNvmApplicationSettings(&settings);
}

void stubNvmSettingsForManuallyDrivenMode(void)
{
	union ApplicationNvmSettings settings =
	{
		.door =
		{
			.mode =
			{
				.isTimeDriven = 0,
				.isManuallyOverridden = 1,
				.isSunEventDriven = 0
			}
		}
	};

	stubNvmApplicationSettings(&settings);
}

void stubNvmSettingsForSunEventDrivenMode(void)
{
	stubNvmSettingsForSunEventDrivenModeWithOffsets(
		(int8_t) anyByte(),
		(int8_t) anyByte());
}

void stubNvmSettingsForSunEventDrivenModeWithOffsets(
	int8_t sunriseMinutes,
	int8_t sunsetMinutes)
{
	union ApplicationNvmSettings settings =
	{
		.door =
		{
			.mode =
			{
				.isTimeDriven = 0,
				.isManuallyOverridden = 0,
				.isSunEventDriven = 1
			},
			.sunEvents =
			{
				.sunriseOffsetMinutes = sunriseMinutes,
				.sunsetOffsetMinutes = sunsetMinutes
			}
		}
	};

	stubNvmApplicationSettings(&settings);
}

void stubNvmSettingsForUnspecifiedMode(void)
{
	union ApplicationNvmSettings settings =
	{
		.door =
		{
			.mode =
			{
				.isTimeDriven = 0,
				.isManuallyOverridden = 0,
				.isSunEventDriven = 0
			}
		}
	};

	stubNvmApplicationSettings(&settings);
}

void stubAnySunEvents(struct SunEventsChanged *eventArgs)
{
	eventArgs->sunrise.hour = anyByteLessThan(24);
	eventArgs->sunrise.minute = anyByteLessThan(60);

	eventArgs->sunset.hour = anyByteLessThan(24);
	eventArgs->sunset.minute = anyByteLessThan(60);
}

void stubDoorWithState(
	enum DoorState state,
	enum DoorTransition transition)
{
	extern struct DoorStateInternal doorState;
	doorState.current = state;
	doorState.transition = transition;
}

void stubMotorIsEnabled(void)
{
	motorIsEnabledReturns = 1;
}

void stubMotorIsDisabled(void)
{
	motorIsEnabledReturns = 0;
}

uint8_t motorIsEnabled(void)
{
	return motorIsEnabledReturns;
}

void motorEnable(void)
{
	motorEnableCalls++;
	motorEnableSequence = ++callSequence;
}

void motorDisable(void)
{
	motorDisableCalls++;
	motorDisableSequence = ++callSequence;
}

void motorOn(int16_t count)
{
	motorOnCalls++;
	motorOnSequence = ++callSequence;
	motorOnArgs[motorOnCalls & 7] = count;
}

void motorOff(void)
{
	motorOffCalls++;
	motorOffSequence = ++callSequence;
}

void motorLimitIsNoLoad(void)
{
	motorLimitIsNoLoadCalls++;
	motorLimitIsNoLoadSequence = ++callSequence;
}

void motorLimitIsMaximumLoad(void)
{
	motorLimitIsMaximumLoadCalls++;
	motorLimitIsMaximumLoadSequence = ++callSequence;
}

void publishDoorAbortedWithAnyFault(void)
{
	static const struct DoorAborted eventArgs = { };
	eventPublish(DOOR_ABORTED, &eventArgs);
}

void publishDateChanged(void)
{
	static const struct DateWithFlags today = { };
	static const struct DateChanged dateChangedEventArgs = { .today = &today };
	eventPublish(DATE_CHANGED, &dateChangedEventArgs);
}

void publishNvmSettingsChanged(void)
{
	static const struct NvmSettingsChanged nvmSettingsChangedEventArgs = { };
	eventPublish(NVM_SETTINGS_CHANGED, &nvmSettingsChangedEventArgs);
}

void publishSunEventsChanged(const struct SunEventsChanged *eventArgs)
{
	eventPublish(SUN_EVENTS_CHANGED, eventArgs);
}

void publishDoorOpenScheduleActioned(void)
{
	static const struct DoorOpenScheduleActioned eventArgs = { };
	eventPublish(DOOR_OPEN_SCHEDULE_ACTIONED, &eventArgs);
}

void publishDoorCloseScheduleActioned(void)
{
	static const struct DoorCloseScheduleActioned eventArgs = { };
	eventPublish(DOOR_CLOSE_SCHEDULE_ACTIONED, &eventArgs);
}

void publishMotorStopped(const struct MotorStopped *eventArgs)
{
	eventPublish(MOTOR_STOPPED, eventArgs);
}

void publishMotorStoppedWithNoFaults(void)
{
	static const struct MotorStopped eventArgs =
	{
		.actualCount = 123,
		.requestedCount = 456,
		.fault = 0
	};

	eventPublish(MOTOR_STOPPED, &eventArgs);
}

void publishMotorStoppedWithFaults(void)
{
	static const struct MotorStopped eventArgs =
	{
		.actualCount = 123,
		.requestedCount = 456,
		.fault = 0xff
	};

	eventPublish(MOTOR_STOPPED, &eventArgs);
}

void assertFarSchedulesAreEqualWithAnyNonNullArgs(
	const struct FarSchedule *expected,
	const struct FarSchedule *actual)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(expected, "NULL1");
	TEST_ASSERT_NOT_NULL_MESSAGE(actual, "NULL2");
	TEST_ASSERT_NOT_NULL_MESSAGE(actual->eventArgs, "NULL3");

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->time.hour,
		actual->time.hour,
		"Hour");

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->time.minute,
		actual->time.minute,
		"Minute");

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		expected->eventType,
		actual->eventType,
		"Event");
}

void farSchedulerAdd(const struct FarSchedule *schedule)
{
	farSchedulerAddSequence[farSchedulerAddCalls & 7] = ++callSequence;
	farSchedulerAddArgs[farSchedulerAddCalls & 7] = schedule;
	farSchedulerAddCalls++;
}

void farSchedulerRemove(const struct FarSchedule *schedule)
{
	farSchedulerRemoveSequence[farSchedulerRemoveCalls & 7] = ++callSequence;
	farSchedulerRemoveArgs[farSchedulerRemoveCalls & 7] = schedule;
	farSchedulerRemoveCalls++;
}

void mockOnDoorAborted(void)
{
	static const struct EventSubscription onDoorAbortedSubscription =
	{
		.type = DOOR_ABORTED,
		.handler = &onDoorAborted,
		.state = (void *) 0
	};

	eventSubscribe(&onDoorAbortedSubscription);
	onDoorAbortedCalls = 0;
}

static void onDoorAborted(const struct Event *event)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(event, "Event");
	TEST_ASSERT_NOT_NULL_MESSAGE(event->args, "Event Args");

	onDoorAbortedArgs[onDoorAbortedCalls & 7] =
		(const struct DoorAborted *) event->args;

	onDoorAbortedCalls++;
}

void mockOnDoorOpened(void)
{
	static const struct EventSubscription onDoorOpenedSubscription =
	{
		.type = DOOR_OPENED,
		.handler = &onDoorOpened,
		.state = (void *) 0
	};

	eventSubscribe(&onDoorOpenedSubscription);
	onDoorOpenedCalls = 0;
}

static void onDoorOpened(const struct Event *event)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(event, "Event");
	TEST_ASSERT_NOT_NULL_MESSAGE(event->args, "Event Args");

	onDoorOpenedArgs[onDoorOpenedCalls & 7] =
		(const struct DoorOpened *) event->args;

	onDoorOpenedCalls++;
}

void mockOnDoorClosed(void)
{
	static const struct EventSubscription onDoorClosedSubscription =
	{
		.type = DOOR_CLOSED,
		.handler = &onDoorClosed,
		.state = (void *) 0
	};

	eventSubscribe(&onDoorClosedSubscription);
	onDoorClosedCalls = 0;
}

static void onDoorClosed(const struct Event *event)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(event, "Event");
	TEST_ASSERT_NOT_NULL_MESSAGE(event->args, "Event Args");

	onDoorClosedArgs[onDoorClosedCalls & 7] =
		(const struct DoorClosed *) event->args;

	onDoorClosedCalls++;
}
