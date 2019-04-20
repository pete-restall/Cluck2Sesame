#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "TestSunEventsScenarios.h"
#include "SunEvents.h"

#include "../NonDeterminism.h"

TEST_FILE("SunEvents/SunEventsInitialise.c")
TEST_FILE("SunEvents/SunEventsCalculate.c")

static uint8_t assertSunEventsChanged;

static volatile uint8_t scenarioIndex;
static uint8_t scenarioIndexLastAsserted;

static volatile uint8_t stubDayOfYearHigh;
static volatile uint8_t stubDayOfYearLow;
static volatile int8_t stubLatitudeOffset;
static volatile int8_t stubLongitudeOffset;

static volatile uint8_t expectedSunriseHour;
static volatile uint8_t expectedSunriseMinute;
static volatile uint8_t expectedSunsetHour;
static volatile uint8_t expectedSunsetMinute;

static struct Event onLocationChangedEvent;
static const struct EventSubscription *onLocationChanged;

static struct Event onDateChangedEvent;
static const struct EventSubscription *onDateChanged;

void setUp(void)
{
	assertSunEventsChanged = 1;
	scenarioIndex = 0;
	scenarioIndexLastAsserted = UINT8_MAX;
	onLocationChanged = (const struct EventSubscription *) 0;
	onDateChanged = (const struct EventSubscription *) 0;
}

void tearDown(void)
{
}

void test_onDateChanged_ExpectSunEventsChangedIsPublishedWithCalculatedSunEventTimes(void)
{
	static struct Location location;
	static const struct LocationChanged locationArgs = { .location = &location };

	static struct Date today;
	static const struct DateChanged dateArgs = { .today = &today };

	sunEventsInitialise();
	onLocationChangedEvent.args = &locationArgs;
	onDateChangedEvent.args = &dateArgs;

	for (scenarioIndex = 0; ; scenarioIndex++)
	{
		today.dayOfYear = (stubDayOfYearHigh << 8) | stubDayOfYearLow;
		if (today.dayOfYear > 365)
			break;

		location.latitudeOffset = stubLatitudeOffset;
		location.longitudeOffset = stubLongitudeOffset;

		// TODO: ANOTHER SET OF TESTS THAT VERIFY THE SAME THINGS, BUT USING THE ORDER OF 'onDateChanged()' FOLLOWED BY 'onLocationChanged()'
		assertSunEventsChanged = 0;
		onLocationChanged->handler(&onLocationChangedEvent);

		assertSunEventsChanged = 1;
		onDateChanged->handler(&onDateChangedEvent);
		TEST_ASSERT_EQUAL_UINT8_MESSAGE(
			scenarioIndex,
			scenarioIndexLastAsserted,
			"No event published");
	}
}

void eventSubscribe(const struct EventSubscription *const subscription)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription, "Null subscription");
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription->handler, "Null handler");
	if (subscription->type == LOCATION_CHANGED)
	{
		onLocationChanged = subscription;
		onLocationChangedEvent.type = subscription->type;
		onLocationChangedEvent.state = subscription->state;
		onLocationChangedEvent.args = (void *) 0;
	}
	else if (subscription->type == DATE_CHANGED)
	{
		onDateChanged = subscription;
		onDateChangedEvent.type = subscription->type;
		onDateChangedEvent.state = subscription->state;
		onDateChangedEvent.args = (void *) 0;
	}
	else
		TEST_FAIL_MESSAGE("Unknown subscription type");
}

void eventPublish(EventType type, const void *const args)
{
	if (assertSunEventsChanged && type == SUN_EVENTS_CHANGED)
	{
		scenarioIndexLastAsserted = scenarioIndex;

		TEST_ASSERT_NOT_NULL_MESSAGE(args, "Null args");

		const struct SunEventsChanged *event =
			(const struct SunEventsChanged *) args;

		TEST_ASSERT_EQUAL_UINT8_MESSAGE(
			expectedSunriseHour,
			event->sunrise.hour,
			"Sunrise hour");

		TEST_ASSERT_EQUAL_UINT8_MESSAGE(
			expectedSunriseMinute,
			event->sunrise.minute,
			"Sunrise minute");

		TEST_ASSERT_EQUAL_UINT8_MESSAGE(
			0,
			event->sunrise.second,
			"Sunrise second");

		TEST_ASSERT_EQUAL_UINT8_MESSAGE(
			expectedSunsetHour,
			event->sunset.hour,
			"Sunset hour");

		TEST_ASSERT_EQUAL_UINT8_MESSAGE(
			expectedSunsetMinute,
			event->sunset.minute,
			"Sunset minute");

		TEST_ASSERT_EQUAL_UINT8_MESSAGE(
			0,
			event->sunset.second,
			"Sunset second");
	}
	else if (type != SUN_EVENTS_CHANGED)
		TEST_FAIL_MESSAGE("Unknown event type");
}
