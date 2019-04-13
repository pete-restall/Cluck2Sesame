#include <unity.h>

#include "Main.h"
#include "Event.h"

#include "Mock_EventHandler.h"
#include "../NonDeterminism.h"

TEST_FILE("Event.c")

#define MAX_EVENTS 8

static struct EventSubscription subscription;
static uint8_t eventState;

static struct EventSubscription anotherSubscription;
static uint8_t anotherEventState;

static int eventHandlerCallCount;

static void eventHandlerThatIncrementsCounter(const struct Event *event)
{
	eventHandlerCallCount++;
}

void setUp(void)
{
	eventHandlerCallCount = 0;

	eventState = anyByte();
	subscription.type = anyByteWithMaskClear(EVENT_TYPE_SYSTEM_MASK);
	subscription.handler = &eventHandler;
	subscription.state = &eventState;

	anotherEventState = anyByte();
	anotherSubscription.type = anyByteExcept(subscription.type) & ~EVENT_TYPE_SYSTEM_MASK;
	anotherSubscription.handler = &anotherEventHandler;
	anotherSubscription.state = &anotherEventState;

	eventInitialise();
}

void tearDown(void)
{
}

void test_eventPublish_called_expectHandlerIsNotCalledBeforeEventDispatch(void)
{
	uint8_t args = anyByte();
	eventHandler_StubWithCallback(eventHandlerThatIncrementsCounter);
	eventSubscribe(&subscription);
	eventPublish(subscription.type, &args);
	TEST_ASSERT_EQUAL_INT8(0, eventHandlerCallCount);
}

void test_eventDispatchNext_calledWhenOneEventPublished_expectHandlerIsCalledWithOldestEvent(void)
{
	struct Event event =
	{
		.type = subscription.type,
		.args = (void *) anyByte(),
		.state = subscription.state
	};

	eventSubscribe(&subscription);
	eventPublish(subscription.type, event.args);

	eventHandler_Expect(&event);
	eventDispatchNext();
}

void test_eventDispatchNext_calledWhenMoreThanOneEventPublished_expectHandlerIsCalledWithOldestEvent(void)
{
	struct Event event =
	{
		.type = subscription.type,
		.args = (void *) anyByte(),
		.state = subscription.state
	};

	struct Event anotherEvent =
	{
		.type = anotherSubscription.type,
		.args = (void *) anyByte(),
		.state = anotherSubscription.state
	};

	eventSubscribe(&subscription);
	eventPublish(event.type, event.args);
	eventPublish(anotherEvent.type, anotherEvent.args);

	eventHandler_Expect(&event);
	anotherEventHandler_StubWithCallback(eventHandlerThatIncrementsCounter);
	eventDispatchNext();
	TEST_ASSERT_EQUAL_INT(0, eventHandlerCallCount);
}

void test_eventDispatchNext_calledTwiceWhenTwoEventsPublished_expectBothHandlersAreCalled(void)
{
	struct Event event =
	{
		.type = subscription.type,
		.args = (void *) anyByte(),
		.state = subscription.state
	};

	struct Event anotherEvent =
	{
		.type = anotherSubscription.type,
		.args = (void *) anyByte(),
		.state = anotherSubscription.state
	};

	eventSubscribe(&subscription);
	eventSubscribe(&anotherSubscription);
	eventPublish(event.type, event.args);
	eventPublish(anotherEvent.type, anotherEvent.args);

	eventHandler_Expect(&event);
	anotherEventHandler_Expect(&anotherEvent);
	eventDispatchNext();
	eventDispatchNext();
}

void test_eventDispatchNext_calledWhenMoreThanOneSubscriberForSameType_expectEachSubscriberHandlerIsCalled(void)
{
	struct Event event =
	{
		.type = subscription.type,
		.args = (void *) anyByte(),
		.state = subscription.state
	};

	struct Event anotherEvent =
	{
		.type = event.type,
		.args = event.args,
		.state = anotherSubscription.state
	};

	anotherSubscription.type = subscription.type;
	eventSubscribe(&subscription);
	eventSubscribe(&anotherSubscription);
	eventPublish(subscription.type, event.args);

	eventHandler_Expect(&event);
	anotherEventHandler_Expect(&anotherEvent);
	eventDispatchNext();
}

void test_eventPublish_calledMoreTimesThanAvailableBufferSize_expectCircularBuffer(void)
{
	uint8_t i;
	for (i = 0; i < MAX_EVENTS; i++)
		eventPublish(subscription.type, NULL);

	eventPublish(anotherSubscription.type, NULL);
	anotherEventHandler_ExpectAnyArgs();

	eventPublish(subscription.type, NULL);
	eventHandler_ExpectAnyArgs();

	eventSubscribe(&subscription);
	eventSubscribe(&anotherSubscription);
	for (i = 0; i < MAX_EVENTS; i++)
		eventDispatchNext();
}

void test_eventDispatchNext_calledMoreTimesThanEvents_expectCorrectNumberOfEventsAreDispatched(void)
{
	eventSubscribe(&subscription);
	eventPublish(subscription.type, NULL);
	eventHandler_ExpectAnyArgs();

	uint8_t i;
	for (i = 0; i < MAX_EVENTS + 1; i++)
		eventDispatchNext();
}

void test_eventDispatchNext_calledWhenNullHandlerAtSubscription_expectNothingHappens(void)
{
	subscription.handler = (EventHandler *) 0;
	eventSubscribe(&subscription);
	eventPublish(subscription.type, NULL);
	eventDispatchNext();
}

void test_eventDispatchNext_calledWhenNullHandlerAfterSubscription_expectNothingHappens(void)
{
	eventSubscribe(&subscription);
	subscription.handler = (EventHandler *) 0;
	eventPublish(subscription.type, NULL);
	eventDispatchNext();
}
