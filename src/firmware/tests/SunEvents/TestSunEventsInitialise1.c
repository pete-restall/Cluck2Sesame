#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Mock_Event.h"
#include "Platform/Nvm.h"
#include "Mock_Location.h"

#include "SunEvents.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

TEST_FILE("SunEvents/SunEventsInitialise.c")
TEST_FILE("SunEvents/SunEventsCalculate.c")

static void ensureEventHandlerDoesNotGetOmittedByTheCompiler(void);

static struct Event onLocationChangedEvent;
static const struct EventSubscription *onLocationChanged;

static void eventSubscribeStub(
	const struct EventSubscription *subscription,
	int numCalls)
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
}

void onBeforeTest(void)
{
	eventSubscribe_StubWithCallback(&eventSubscribeStub);
	onLocationChanged = (const struct EventSubscription *) 0;
}

void onAfterTest(void)
{
}

void test_sunEventsInitialise_called_expectSubscriptionToLocationChanged(void)
{
	sunEventsInitialise();
	ensureEventHandlerDoesNotGetOmittedByTheCompiler();
	TEST_ASSERT_NOT_NULL(onLocationChanged);
}

static void ensureEventHandlerDoesNotGetOmittedByTheCompiler(void)
{
	static volatile uint8_t dummy = 0;
	if (onLocationChanged && dummy)
		onLocationChanged->handler(&onLocationChangedEvent);
}
