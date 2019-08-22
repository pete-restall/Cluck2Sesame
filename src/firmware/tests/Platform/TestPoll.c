#include <xc.h>
#include <unity.h>

#include "Platform/Main.h"
#include "Mock_Event.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

TEST_FILE("Platform/Poll.c")

static int numberOfEventPublishCalls;

const struct Event eventEmptyArgs = { };

static void eventPublishThatIncrementsCounter(
	EventType type,
	const void *args,
	int numCalls)
{
	numberOfEventPublishCalls++;
}

void onBeforeTest(void)
{
	numberOfEventPublishCalls = 0;
}

void onAfterTest(void)
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
