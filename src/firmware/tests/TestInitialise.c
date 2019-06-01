#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Main.h"
#include "Platform/Mock_Event.h"

#include "Fixture.h"

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
static struct CallDetails powerManagementInitialiseCalls;
static struct CallDetails clockInitialiseCalls;
static struct CallDetails nearSchedulerInitialiseCalls;
static struct CallDetails farSchedulerInitialiseCalls;
static struct CallDetails voltageRegulatorInitialiseCalls;
static struct CallDetails pwmTimerInitialiseCalls;
static struct CallDetails lcdInitialiseCalls;
static struct CallDetails motorInitialiseCalls;
static struct CallDetails sunEventsInitialiseCalls;
static struct CallDetails systemInitialisedEventPublishCalls;

void onBeforeTest(void)
{
	callSequence = 1;
	clearCallsFor(&eventInitialiseCalls);
	clearCallsFor(&powerManagementInitialiseCalls);
	clearCallsFor(&clockInitialiseCalls);
	clearCallsFor(&nearSchedulerInitialiseCalls);
	clearCallsFor(&farSchedulerInitialiseCalls);
	clearCallsFor(&voltageRegulatorInitialiseCalls);
	clearCallsFor(&pwmTimerInitialiseCalls);
	clearCallsFor(&lcdInitialiseCalls);
	clearCallsFor(&motorInitialiseCalls);
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

void onAfterTest(void)
{
}

void test_initialise_called_expectEventSystemIsInitialisedFirst(void)
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

void test_initialise_called_expectPowerManagementIsInitialisedAfterEvents(void)
{
	initialise();
	assertCalledOnceAtSequence(&powerManagementInitialiseCalls, 2);
}

void powerManagementInitialise(void)
{
	registerCallFor(&powerManagementInitialiseCalls);
}

void test_initialise_called_expectClockIsInitialisedAfterPowerManagement(void)
{
	initialise();
	assertCalledOnceAtSequence(&clockInitialiseCalls, 3);
}

void clockInitialise(void)
{
	registerCallFor(&clockInitialiseCalls);
}

void test_initialise_called_expectNearSchedulerIsInitialisedAfterClock(void)
{
	initialise();
	assertCalledOnceAtSequence(&nearSchedulerInitialiseCalls, 4);
}

void nearSchedulerInitialise(void)
{
	registerCallFor(&nearSchedulerInitialiseCalls);
}

void test_initialise_called_expectFarSchedulerIsInitialisedAfterNearScheduler(void)
{
	initialise();
	assertCalledOnceAtSequence(&farSchedulerInitialiseCalls, 5);
}

void farSchedulerInitialise(void)
{
	registerCallFor(&farSchedulerInitialiseCalls);
}

void test_initialise_called_expectVoltageRegulatorIsInitialisedAfterFarScheduler(void)
{
	initialise();
	assertCalledOnceAtSequence(&voltageRegulatorInitialiseCalls, 6);
}

void voltageRegulatorInitialise(void)
{
	registerCallFor(&voltageRegulatorInitialiseCalls);
}

void test_initialise_called_expectPwmTimerIsInitialisedAfterVoltageRegulator(void)
{
	initialise();
	assertCalledOnceAtSequence(&pwmTimerInitialiseCalls, 7);
}

void pwmTimerInitialise(void)
{
	registerCallFor(&pwmTimerInitialiseCalls);
}

void test_initialise_called_expectLcdIsInitialisedAfterPwmTimer(void)
{
	initialise();
	assertCalledOnceAtSequence(&lcdInitialiseCalls, 8);
}

void lcdInitialise(void)
{
	registerCallFor(&lcdInitialiseCalls);
}

void test_initialise_called_expectMotorIsInitialisedAfterLcd(void)
{
	initialise();
	assertCalledOnceAtSequence(&motorInitialiseCalls, 9);
}

void motorInitialise(void)
{
	registerCallFor(&motorInitialiseCalls);
}

void test_initialise_called_expectSunEventsAreInitialisedAfterMotor(void)
{
	initialise();
	assertCalledOnceAtSequence(&sunEventsInitialiseCalls, 10);
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
