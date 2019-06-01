#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Main.h"
#include "Platform/Event.h"

#include "Mock_EventHandler.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

TEST_FILE("Platform/Event.c")

#define MAX_EVENTS 16

static struct EventSubscription subscription;
static uint8_t eventState;

static struct EventSubscription anotherSubscription;
static uint8_t anotherEventState;

void onBeforeTest(void)
{
	eventState = anyByte();
	subscription.type = anyByte();
	subscription.handler = &eventHandler;
	subscription.state = &eventState;

	anotherEventState = anyByte();
	anotherSubscription.type = anyByteExcept(subscription.type);
	anotherSubscription.handler = &anotherEventHandler;
	anotherSubscription.state = &anotherEventState;

	eventInitialise();
}

void onAfterTest(void)
{
}

void test_eventPublish_calledMoreTimesThanAvailableBufferSize_expectCircularBuffer(void)
{
	for (uint8_t i = 0; i < MAX_EVENTS; i++)
		eventPublish(subscription.type, NULL);

	eventPublish(anotherSubscription.type, NULL);
	anotherEventHandler_ExpectAnyArgs();

	eventPublish(subscription.type, NULL);
	eventHandler_ExpectAnyArgs();

	eventSubscribe(&subscription);
	eventSubscribe(&anotherSubscription);
	for (uint8_t i = 0; i < MAX_EVENTS; i++)
		eventDispatchNext();
}

void test_eventDispatchNext_calledMoreTimesThanEvents_expectCorrectNumberOfEventsAreDispatched(void)
{
	eventSubscribe(&subscription);
	eventPublish(subscription.type, NULL);
	eventHandler_ExpectAnyArgs();

	for (uint8_t i = 0; i < MAX_EVENTS + 1; i++)
		eventDispatchNext();
}

void test_eventDispatchNext_calledWhenEventsDispatched_ExpectNonZeroIsReturned(void)
{
	eventPublish(subscription.type, NULL);
	TEST_ASSERT_NOT_EQUAL(0, eventDispatchNext());
}

void test_eventDispatchNext_calledWhenEventsNotDispatched_ExpectZeroIsReturned(void)
{
	eventPublish(subscription.type, NULL);
	eventDispatchNext();
	TEST_ASSERT_EQUAL_INT8(0, eventDispatchNext());
}
