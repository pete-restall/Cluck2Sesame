#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Main.h"
#include "Mock_Event.h"

#include "Fixture.h"

TEST_FILE("Platform/Initialise.c")

struct CallDetails
{
	uint8_t sequence;
	uint8_t count;
};

void eventInitialiseCallback(int numCalls);
void eventPublishCallback(EventType type, const void *args, int numCalls);

static void clearCallsFor(struct CallDetails *calls);

static void assertCalledOnceAtSequence(
	struct CallDetails *call,
	uint8_t sequence);

static void registerCallFor(struct CallDetails *calls);

static uint8_t callSequence;
static struct CallDetails eventInitialiseCalls;
static struct CallDetails powerManagementInitialiseCalls;
static struct CallDetails clockInitialiseCalls;
static struct CallDetails nearSchedulerInitialiseCalls;
static struct CallDetails farSchedulerInitialiseCalls;
static struct CallDetails voltageRegulatorInitialiseCalls;
static struct CallDetails temperatureInitialiseCalls;
static struct CallDetails batteryInitialiseCalls;
static struct CallDetails pwmTimerInitialiseCalls;
static struct CallDetails adcInitialiseCalls;
static struct CallDetails lcdInitialiseCalls;
static struct CallDetails motorInitialiseCalls;
static struct CallDetails periodicMonitorInitialiseCalls;
static struct CallDetails buttonsInitialiseCalls;
static struct CallDetails calibrationModeInitialiseCalls;
static struct CallDetails applicationInitialiseCalls;
static struct CallDetails systemInitialisedEventPublishCalls;

const struct Event eventEmptyArgs = { };

void onBeforeTest(void)
{
	callSequence = 1;
	clearCallsFor(&eventInitialiseCalls);
	clearCallsFor(&powerManagementInitialiseCalls);
	clearCallsFor(&clockInitialiseCalls);
	clearCallsFor(&nearSchedulerInitialiseCalls);
	clearCallsFor(&farSchedulerInitialiseCalls);
	clearCallsFor(&voltageRegulatorInitialiseCalls);
	clearCallsFor(&temperatureInitialiseCalls);
	clearCallsFor(&batteryInitialiseCalls);
	clearCallsFor(&pwmTimerInitialiseCalls);
	clearCallsFor(&adcInitialiseCalls);
	clearCallsFor(&lcdInitialiseCalls);
	clearCallsFor(&motorInitialiseCalls);
	clearCallsFor(&periodicMonitorInitialiseCalls);
	clearCallsFor(&buttonsInitialiseCalls);
	clearCallsFor(&calibrationModeInitialiseCalls);
	clearCallsFor(&applicationInitialiseCalls);
	clearCallsFor(&systemInitialisedEventPublishCalls);

	eventInitialise_StubWithCallback(&eventInitialiseCallback);
	eventPublish_StubWithCallback(&eventPublishCallback);
}

static void clearCallsFor(struct CallDetails *calls)
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
	struct CallDetails *call,
	uint8_t sequence)
{
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, call->count, "Count");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(sequence, call->sequence, "Sequence");
}

void eventInitialiseCallback(int numCalls)
{
	registerCallFor(&eventInitialiseCalls);
}

static void registerCallFor(struct CallDetails *calls)
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

void test_initialise_called_expectTemperatureIsInitialisedAfterVoltageRegulator(void)
{
	initialise();
	assertCalledOnceAtSequence(&temperatureInitialiseCalls, 7);
}

void temperatureInitialise(void)
{
	registerCallFor(&temperatureInitialiseCalls);
}

void test_initialise_called_expectBatteryIsInitialisedAfterTemperature(void)
{
	initialise();
	assertCalledOnceAtSequence(&batteryInitialiseCalls, 8);
}

void batteryInitialise(void)
{
	registerCallFor(&batteryInitialiseCalls);
}

void test_initialise_called_expectPwmTimerIsInitialisedAfterBattery(void)
{
	initialise();
	assertCalledOnceAtSequence(&pwmTimerInitialiseCalls, 9);
}

void pwmTimerInitialise(void)
{
	registerCallFor(&pwmTimerInitialiseCalls);
}

void test_initialise_called_expectAdcIsInitialisedAfterPwmTimer(void)
{
	initialise();
	assertCalledOnceAtSequence(&adcInitialiseCalls, 10);
}

void adcInitialise(void)
{
	registerCallFor(&adcInitialiseCalls);
}

void test_initialise_called_expectLcdIsInitialisedAfterAdc(void)
{
	initialise();
	assertCalledOnceAtSequence(&lcdInitialiseCalls, 11);
}

void lcdInitialise(void)
{
	registerCallFor(&lcdInitialiseCalls);
}

void test_initialise_called_expectMotorIsInitialisedAfterLcd(void)
{
	initialise();
	assertCalledOnceAtSequence(&motorInitialiseCalls, 12);
}

void motorInitialise(void)
{
	registerCallFor(&motorInitialiseCalls);
}

void test_initialise_called_expectPeriodicMonitorIsInitialisedAfterMotor(void)
{
	initialise();
	assertCalledOnceAtSequence(&periodicMonitorInitialiseCalls, 13);
}

void periodicMonitorInitialise(void)
{
	registerCallFor(&periodicMonitorInitialiseCalls);
}

void test_initialise_called_expectButtonsAreInitialisedAfterPeriodicMonitor(void)
{
	initialise();
	assertCalledOnceAtSequence(&buttonsInitialiseCalls, 14);
}

void buttonsInitialise(void)
{
	registerCallFor(&buttonsInitialiseCalls);
}

void test_initialise_called_expectCalibrationModeIsInitialisedAfterButtons(void)
{
	initialise();
	assertCalledOnceAtSequence(&calibrationModeInitialiseCalls, 15);
}

void calibrationModeInitialise(void)
{
	registerCallFor(&calibrationModeInitialiseCalls);
}

void test_initialise_called_expectApplicationIsInitialisedAfterCalibrationMode(void)
{
	initialise();
	assertCalledOnceAtSequence(&applicationInitialiseCalls, 16);
}

void applicationInitialise(void)
{
	registerCallFor(&applicationInitialiseCalls);
}

void test_initialise_called_expectSystemInitialisedEventIsPublishedLast(void)
{
	initialise();
	assertCalledOnceAtSequence(
		&systemInitialisedEventPublishCalls,
		callSequence - 1);
}

void eventPublishCallback(EventType type, const void *args, int numCalls)
{
	if (type == SYSTEM_INITIALISED && args)
		registerCallFor(&systemInitialisedEventPublishCalls);
}
