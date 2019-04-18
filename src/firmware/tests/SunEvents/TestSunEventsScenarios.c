#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Mock_Event.h"
#include "Mock_Clock.h"

#include "SunEvents.h"

#include "../NonDeterminism.h"

TEST_FILE("SunEvents/SunEventsInitialise.c")
TEST_FILE("SunEvents/SunEventsCalculate.c")

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

static struct Event onDateChangedEvent;
static const struct EventSubscription *onDateChanged;

static void eventPublishAssertingScenario(
	EventType type,
	void *args,
	int numCalls);

static void eventSubscribeStub(
	const struct EventSubscription *const subscription,
	int numCalls)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription, "Null subscription");
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription->handler, "Null handler");
	if (subscription->type == DATE_CHANGED)
	{
		onDateChanged = subscription;
		onDateChangedEvent.type = subscription->type;
		onDateChangedEvent.state = subscription->state;
		onDateChangedEvent.args = (void *) 0;
	}
	else
		TEST_FAIL_MESSAGE("Unknown subscription type");
}

void setUp(void)
{
	scenarioIndex = 0;
	scenarioIndexLastAsserted = UINT8_MAX;
	eventSubscribe_StubWithCallback(&eventSubscribeStub);
	eventPublish_StubWithCallback(&eventPublishAssertingScenario);
	onDateChanged = (const struct EventSubscription *) 0;
}

void tearDown(void)
{
}

void test_onDateChanged_ExpectSunEventsChangedIsPublishedWithCalculatedSunEventTimes(void)
{
	static struct Date today;
	static const struct DateChanged date = { .today = &today };

	sunEventsInitialise();
	onDateChangedEvent.args = &date;

	today.year = today.month = today.day = 0;
	today.flags.isLeapYear = 0;
	today.flags.isDaylightSavings = 0;

	for (scenarioIndex = 0; ; scenarioIndex++)
	{
		today.dayOfYear = (stubDayOfYearHigh << 8) | stubDayOfYearLow;
		if (today.dayOfYear > 365)
			break;

		onDateChanged->handler(&onDateChangedEvent);
		TEST_ASSERT_EQUAL_UINT8_MESSAGE(
			scenarioIndex,
			scenarioIndexLastAsserted,
			"No event published");
	}
}

static void eventPublishAssertingScenario(
	EventType type,
	void *args,
	int numCalls)
{
	if (type == SUN_EVENTS_CHANGED)
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
	else
		TEST_FAIL_MESSAGE("Unknown event type");
}
