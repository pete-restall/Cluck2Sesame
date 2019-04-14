#include <unity.h>

#include "Main.h"
#include "Event.h"

#include "Mock_EventHandler.h"
#include "../NonDeterminism.h"

TEST_FILE("Event.c")

#define MAX_EVENTS 16

static struct EventSubscription subscription;
static uint8_t eventState;

static struct EventSubscription anotherSubscription;
static uint8_t anotherEventState;

void setUp(void)
{
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
