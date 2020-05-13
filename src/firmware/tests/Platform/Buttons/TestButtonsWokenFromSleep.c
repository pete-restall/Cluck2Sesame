#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Mock_Event.h"
#include "Mock_PowerManagement.h"
#include "Mock_NearScheduler.h"

#include "Platform/Buttons.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Buttons.c")

static void eventSubscribeStub(const struct EventSubscription *subscription, int numCalls);
static void wakeFromSleep(void);

static EventHandler onWokenFromSleep;
static void *onWokenFromSleepState;

void onBeforeTest(void)
{
	onWokenFromSleep = (EventHandler) 0;
	onWokenFromSleepState = (void *) 0;
	eventSubscribe_StubWithCallback(&eventSubscribeStub);
	buttonsInitialise();
	IOCAF = 0;
}

static void eventSubscribeStub(const struct EventSubscription *subscription, int numCalls)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription, "Null subscription");
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription->handler, "Null handler");
	if (subscription->type == WOKEN_FROM_SLEEP)
	{
		onWokenFromSleep = subscription->handler;
		onWokenFromSleepState = subscription->state;
	}
}

void onAfterTest(void)
{
}

// TODO: The simulator's being a bit of a pain with IOC - wasted so much time.  Leaving it to bench testing...
void IGNORED__test_wokenFromSleep_onPublishedWhenIocFlagsSet_expectAllIocFlagsAreCleared(void)
{
	IOCAF = anyByteWithMaskSet(_IOCAF_IOCAF0_MASK | _IOCAF_IOCAF1_MASK);
	wakeFromSleep();
	TEST_ASSERT_EQUAL_UINT8(0, IOCAF);
}

static void wakeFromSleep(void)
{
	if (!onWokenFromSleep)
		return;

	struct Event eventArgs =
	{
		.type = WOKEN_FROM_SLEEP,
		.args = (const void *) 0,
		.state = onWokenFromSleepState
	};

	onWokenFromSleep(&eventArgs);
}

void test_wokenFromSleep_onPublishedWhenIocFlagsSet_expectIocOredFlagIsCleared(void)
{
	IOCAF = anyByteWithMaskSet(_IOCAF_IOCAF0_MASK | _IOCAF_IOCAF1_MASK);
	PIR0 = anyByteWithMaskSet(_PIR0_IOCIF_MASK);
	uint8_t originalPir0 = PIR0;
	wakeFromSleep();
	TEST_ASSERT_EQUAL_UINT8(originalPir0 & ~_PIR0_IOCIF_MASK, PIR0);
}

// TODO: EXPECT NEAR SCHEDULE IS ADDED / UPDATED (ONLY WHEN IOCAF IS NOT 0) - IOC IS TOO DIFFICULT TO WORK WITH IN THE SIMULATOR

// TODO: NEAR SCHEDULE EXPIRY TESTS... - IOC IS TOO DIFFICULT TO WORK WITH IN THE SIMULATOR
