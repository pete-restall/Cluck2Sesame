#include <xc.h>
#include <unity.h>

#include "Main.h"
#include "Mock_Event.h"

#include "../NonDeterminism.h"

TEST_FILE("Poll.c")

static int numberOfEventPublishCalls;

static void eventPublishThatIncrementsCounter(
	EventType type,
	const void *const args,
	int numCalls)
{
	numberOfEventPublishCalls++;
}

void setUp(void)
{
	numberOfEventPublishCalls = 0;
}

void tearDown(void)
{
}

void test_poll_calledWhenNextEventDispatched_expectAllEventsDispatchedEventIsNotPublished(void)
{
	eventDispatchNext_ExpectAndReturn(1);
	eventPublish_StubWithCallback(eventPublishThatIncrementsCounter);
	poll();
	TEST_ASSERT_EQUAL_INT(0, numberOfEventPublishCalls);
}

void test_poll_calledWhenNextEventNotDispatched_expectAllEventsDispatchedEventIsPublished(void)
{
	static const struct AllEventsDispatched emptyArgs = { };
	eventDispatchNext_ExpectAndReturn(0);
	eventPublish_Expect(ALL_EVENTS_DISPATCHED, &emptyArgs);
	poll();
}
