#include <xc.h>
#include <unity.h>

#include "Main.h"
#include "Mock_Event.h"
#include "PowerManagement.h"

#include "NonDeterminism.h"

TEST_FILE("Poll.c")

static volatile uint8_t sleepExecuted;
static volatile uint8_t pie0BeforeSleep;
static volatile uint8_t vregconBeforeSleep;

static struct Event onAllEventsDispatchedEvent;
static const struct EventSubscription *onAllEventsDispatched;

static void eventSubscribeStub(
	const struct EventSubscription *const subscription,
	int numCalls)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription, "Null subscription");
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription->handler, "Null handler");
	if (subscription->type == ALL_EVENTS_DISPATCHED)
	{
		onAllEventsDispatched = subscription;
		onAllEventsDispatchedEvent.type = subscription->type;
		onAllEventsDispatchedEvent.state = subscription->state;
		onAllEventsDispatchedEvent.args = (void *) 0;
	}
	else
		TEST_FAIL_MESSAGE("Unknown subscription type");
}

void setUp(void)
{
	sleepExecuted = 0;
	eventSubscribe_StubWithCallback(&eventSubscribeStub);
	onAllEventsDispatched = (const struct EventSubscription *) 0;
}

void tearDown(void)
{
}

void test_powerManagementInitialise_called_expectSubscriptionToAllEventsDispatched(void)
{
	powerManagementInitialise();
	TEST_ASSERT_NOT_NULL(onAllEventsDispatched);
}

void test_onAllEventsDispatched_ExpectDeviceSleeps(void)
{
	powerManagementInitialise();
	onAllEventsDispatched->handler(&onAllEventsDispatchedEvent);
	TEST_ASSERT_NOT_EQUAL(0, sleepExecuted);
}

void test_onAllEventsDispatched_ExpectDeviceEntersLowPowerSleep(void)
{
	powerManagementInitialise();
	VREGCON = anyByteWithMaskClear(_VREGCON_VREGPM_MASK);
	vregconBeforeSleep = VREGCON;
	onAllEventsDispatched->handler(&onAllEventsDispatchedEvent);
	TEST_ASSERT_BIT_HIGH(_VREGCON_VREGPM_POSITION, vregconBeforeSleep);
}

void test_onAllEventsDispatched_ExpectWokenFromSleepIsPublished(void)
{
	static const struct WokenFromSleep emptyArgs = { };
	powerManagementInitialise();
	eventPublish_Expect(WOKEN_FROM_SLEEP, &emptyArgs);
	onAllEventsDispatched->handler(&onAllEventsDispatchedEvent);
}
