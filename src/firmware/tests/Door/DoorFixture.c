#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/NvmSettings.h"
#include "ApplicationNvmSettings.h"
#include "SunEvents.h"
#include "Door.h"

#include "DoorFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"
#include "../NvmSettingsFixture.h"

static uint8_t callSequence;

uint8_t farSchedulerAddCalls;
uint8_t farSchedulerAddSequence[8];
const struct FarSchedule *farSchedulerAddArgs[8];

uint8_t farSchedulerRemoveCalls;
uint8_t farSchedulerRemoveSequence[8];
const struct FarSchedule *farSchedulerRemoveArgs[8];

void doorFixtureInitialise(void)
{
	callSequence = 0;
	farSchedulerAddCalls = 0;
	farSchedulerRemoveCalls = 0;
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
