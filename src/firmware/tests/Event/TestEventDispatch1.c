#include <xc.h>
#include <unity.h>

#include "Main.h"
#include "Event.h"

#include "Mock_EventHandler.h"
#include "NonDeterminism.h"

TEST_FILE("Event.c")

static struct EventSubscription subscription;
static uint8_t eventState;

static struct EventSubscription anotherSubscription;
static uint8_t anotherEventState;

static int eventHandlerCallCount;

static void eventHandlerThatIncrementsCounter(
	const struct Event *event,
	int numCalls)
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
	TEST_ASSERT_EQUAL_INT(0, eventHandlerCallCount);
}

void test_eventDispatchNext_calledWhenOneEventPublished_expectHandlerIsCalledWithOldestEvent(void)
{
	struct Event event =
	{
		.type = subscription.type,
		.args = (void *) (int) anyByte(),
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
		.args = (void *) (int) anyByte(),
		.state = subscription.state
	};

	struct Event anotherEvent =
	{
		.type = anotherSubscription.type,
		.args = (void *) (int) anyByte(),
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
		.args = (void *) (int) anyByte(),
		.state = subscription.state
	};

	struct Event anotherEvent =
	{
		.type = anotherSubscription.type,
		.args = (void *) (int) anyByte(),
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
		.args = (void *) (int) anyByte(),
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

void test_eventDispatchNext_calledWhenNullHandlerAtSubscription_expectNothingHappens(void)
{
	subscription.handler = (EventHandler) 0;
	eventSubscribe(&subscription);
	eventPublish(subscription.type, NULL);
	eventDispatchNext();
}

void test_eventDispatchNext_calledWhenNullHandlerAfterSubscription_expectNothingHappens(void)
{
	eventSubscribe(&subscription);
	subscription.handler = (EventHandler) 0;
	eventPublish(subscription.type, NULL);
	eventDispatchNext();
}
