#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Main.h"

#include "Fixture.h"

TEST_FILE("ApplicationInitialise.c")

struct CallDetails
{
	uint8_t sequence;
	uint8_t count;
};

static void clearCallsFor(struct CallDetails *calls);
static void assertCalledOnceAtSequence(struct CallDetails *call, uint8_t sequence);
static void registerCallFor(struct CallDetails *calls);

static uint8_t callSequence;
static struct CallDetails locationInitialiseCalls;
static struct CallDetails sunEventsInitialiseCalls;
static struct CallDetails doorInitialiseCalls;
static struct CallDetails uiInitialiseCalls;

void onBeforeTest(void)
{
	callSequence = 1;
	clearCallsFor(&locationInitialiseCalls);
	clearCallsFor(&sunEventsInitialiseCalls);
	clearCallsFor(&doorInitialiseCalls);
	clearCallsFor(&uiInitialiseCalls);
}

static void clearCallsFor(struct CallDetails *calls)
{
	calls->sequence = 0;
	calls->count = 0;
}

void onAfterTest(void)
{
}

void test_applicationInitialise_called_expectLocationIsInitialisedFirst(void)
{
	applicationInitialise();
	assertCalledOnceAtSequence(&locationInitialiseCalls, 1);
}

static void assertCalledOnceAtSequence(struct CallDetails *call, uint8_t sequence)
{
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, call->count, "Count");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(sequence, call->sequence, "Sequence");
}

void locationInitialise(void)
{
	registerCallFor(&locationInitialiseCalls);
}

static void registerCallFor(struct CallDetails *calls)
{
	calls->sequence = callSequence++;
	calls->count++;
}

void test_applicationInitialise_called_expectSunEventsAreInitialisedAfterLocation(void)
{
	applicationInitialise();
	assertCalledOnceAtSequence(&sunEventsInitialiseCalls, 2);
}

void sunEventsInitialise(void)
{
	registerCallFor(&sunEventsInitialiseCalls);
}

void test_applicationInitialise_called_expectDoorIsInitialisedAfterSunEvents(void)
{
	applicationInitialise();
	assertCalledOnceAtSequence(&doorInitialiseCalls, 3);
}

void doorInitialise(void)
{
	registerCallFor(&doorInitialiseCalls);
}

void test_applicationInitialise_called_expectUiIsInitialisedAfterDoor(void)
{
	applicationInitialise();
	assertCalledOnceAtSequence(&uiInitialiseCalls, 4);
}

void uiInitialise(void)
{
	registerCallFor(&uiInitialiseCalls);
}
