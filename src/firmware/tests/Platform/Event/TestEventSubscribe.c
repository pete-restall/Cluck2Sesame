#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Main.h"
#include "Platform/Event.h"

#include "Mock_EventHandler.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Event.c")

#define MAX_SUBSCRIPTIONS 32

static struct EventSubscription subscription;
static uint8_t eventState;

static struct EventSubscription anotherSubscription;
static uint8_t anotherEventState;

static struct EventSubscription lotsOfSubscriptions[MAX_SUBSCRIPTIONS];
static uint8_t lotsOfEventStates[MAX_SUBSCRIPTIONS];

static int eventHandlerCallCount;

static void eventHandlerThatIncrementsCounter(const struct Event *event)
{
	eventHandlerCallCount++;
}

void onBeforeTest(void)
{
	eventHandlerCallCount = 0;

	eventState = anyByte();
	subscription.type = 1;
	subscription.handler = &eventHandler;
	subscription.state = &eventState;

	anotherEventState = anyByte();
	anotherSubscription.type = 2;
	anotherSubscription.handler = &anotherEventHandler;
	anotherSubscription.state = &anotherEventState;

	for (uint8_t i = 0; i < MAX_SUBSCRIPTIONS; i++)
	{
		lotsOfEventStates[i] = anyByte();
		lotsOfSubscriptions[i].type = 3;
		lotsOfSubscriptions[i].handler = &eventHandlerThatIncrementsCounter;
		lotsOfSubscriptions[i].state = &lotsOfEventStates[i];
	}

	eventInitialise();
}

void onAfterTest(void)
{
}

void test_eventSubscribe_calledWithExistingSubscription_expectOnlyOneDispatch(void)
{
	eventSubscribe(&subscription);
	eventSubscribe(&subscription);
	eventPublish(subscription.type, NULL);

	eventHandler_ExpectAnyArgs();
	eventDispatchNext();
}

void test_eventSubscribe_calledMoreTimesThanBufferSize_expectExistingSubscriptionsAreNotOverwritten(void)
{
	subscription.type = lotsOfSubscriptions[0].type;
	eventSubscribe(&subscription);
	eventHandler_ExpectAnyArgs();

	for (uint8_t i = 0; i < MAX_SUBSCRIPTIONS; i++)
		eventSubscribe(&lotsOfSubscriptions[i]);

	eventPublish(subscription.type, NULL);
	eventDispatchNext();

	TEST_ASSERT_EQUAL_UINT8(MAX_SUBSCRIPTIONS - 1, eventHandlerCallCount);
}

void test_eventUnsubscribe_called_expectEventsAreNotDispatchedAnymore(void)
{
	eventSubscribe(&subscription);
	eventPublish(subscription.type, NULL);
	eventPublish(subscription.type, NULL);

	eventHandler_ExpectAnyArgs();
	eventDispatchNext();
	eventUnsubscribe(&subscription);
	eventDispatchNext();
}

void test_eventUnsubscribe_called_expectHoleInListCanBeRefilledByNewSubscription(void)
{
	for (uint8_t i = 0; i < MAX_SUBSCRIPTIONS; i++)
		eventSubscribe(&lotsOfSubscriptions[i]);

	eventUnsubscribe(&lotsOfSubscriptions[7]);

	subscription.type = lotsOfSubscriptions[0].type;
	eventSubscribe(&subscription);
	eventHandler_ExpectAnyArgs();

	eventPublish(subscription.type, NULL);
	eventDispatchNext();

	TEST_ASSERT_EQUAL_UINT8(MAX_SUBSCRIPTIONS - 1, eventHandlerCallCount);
}
