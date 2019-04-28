#include <xc.h>
#include <unity.h>

#include "Main.h"
#include "Mock_Event.h"

TEST_FILE("Initialise.c")

struct CallDetails
{
	uint8_t sequence;
	uint8_t count;
};

void eventInitialiseCallback(int numCalls);
void eventPublishCallback(EventType type, const void *const args, int numCalls);

static void clearCallsFor(struct CallDetails *const calls);

static void assertCalledOnceAtSequence(
	struct CallDetails *const call,
	uint8_t sequence);

static void registerCallFor(struct CallDetails *const calls);

static uint8_t callSequence;
static struct CallDetails eventInitialiseCalls;
static struct CallDetails clockInitialiseCalls;
static struct CallDetails nearSchedulerInitialiseCalls;
static struct CallDetails powerManagementInitialiseCalls;
static struct CallDetails sunEventsInitialiseCalls;
static struct CallDetails systemInitialisedEventPublishCalls;

void setUp(void)
{
	callSequence = 1;
	clearCallsFor(&eventInitialiseCalls);
	clearCallsFor(&clockInitialiseCalls);
	clearCallsFor(&nearSchedulerInitialiseCalls);
	clearCallsFor(&powerManagementInitialiseCalls);
	clearCallsFor(&sunEventsInitialiseCalls);
	clearCallsFor(&systemInitialisedEventPublishCalls);

	eventInitialise_StubWithCallback(&eventInitialiseCallback);
	eventPublish_StubWithCallback(&eventPublishCallback);
}

static void clearCallsFor(struct CallDetails *const calls)
{
	calls->sequence = 0;
	calls->count = 0;
}

void tearDown(void)
{
}

void test_initialise_called_expectEventSystemIsInitialised(void)
{
	initialise();
	assertCalledOnceAtSequence(&eventInitialiseCalls, 1);
}

static void assertCalledOnceAtSequence(
	struct CallDetails *const call,
	uint8_t sequence)
{
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, call->count, "Count");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(sequence, call->sequence, "Sequence");
}

void eventInitialiseCallback(int numCalls)
{
	registerCallFor(&eventInitialiseCalls);
}

static void registerCallFor(struct CallDetails *const calls)
{
	calls->sequence = callSequence++;
	calls->count++;
}

void test_initialise_called_expectClockIsInitialisedAfterEvents(void)
{
	initialise();
	assertCalledOnceAtSequence(&clockInitialiseCalls, 2);
}

void clockInitialise(void)
{
	registerCallFor(&clockInitialiseCalls);
}

void test_initialise_called_expectNearSchedulerIsInitialisedAfterClock(void)
{
	initialise();
	assertCalledOnceAtSequence(&nearSchedulerInitialiseCalls, 3);
}

void nearSchedulerInitialise(void)
{
	registerCallFor(&nearSchedulerInitialiseCalls);
}

void test_initialise_called_expectPowerManagementIsInitialisedAfterNearScheduler(void)
{
	initialise();
	assertCalledOnceAtSequence(&powerManagementInitialiseCalls, 4);
}

void powerManagementInitialise(void)
{
	registerCallFor(&powerManagementInitialiseCalls);
}

void test_initialise_called_expectSunEventsAreInitialisedAfterPowerManagement(void)
{
	initialise();
	assertCalledOnceAtSequence(&sunEventsInitialiseCalls, 5);
}

void sunEventsInitialise(void)
{
	registerCallFor(&sunEventsInitialiseCalls);
}

void test_initialise_called_expectSystemInitialisedEventIsPublishedLast(void)
{
	initialise();
	assertCalledOnceAtSequence(
		&systemInitialisedEventPublishCalls,
		callSequence - 1);
}

void eventPublishCallback(EventType type, const void *const args, int numCalls)
{
	if (type == SYSTEM_INITIALISED && args)
		registerCallFor(&systemInitialisedEventPublishCalls);
}
