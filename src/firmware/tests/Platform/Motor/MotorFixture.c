#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/PowerManagement.h"
#include "Platform/NearScheduler.h"
#include "Platform/VoltageRegulator.h"
#include "Platform/Motor.h"

#include "../../NonDeterminism.h"
#include "../../Fixture.h"

#include "MotorFixture.h"

static void onMotorEnabled(const struct Event *event);
static void onMotorDisabled(const struct Event *event);
static void onVoltageRegulatorDisabled(const struct Event *event);
static void onMotorStarted(const struct Event *event);
static void onMotorStopped(const struct Event *event);

static uint8_t callSequence;
static uint8_t voltageRegulatorDisableIsStubbedForEvent;

uint8_t pwmTimerEnableCalls;
uint8_t pwmTimerDisableCalls;
uint8_t voltageRegulatorIsEnabledValue;
uint8_t voltageRegulatorEnableCalls;
uint8_t voltageRegulatorEnableSequence;
uint8_t voltageRegulatorDisableCalls;
uint8_t voltageRegulatorDisableSequence;
uint8_t onVoltageRegulatorDisabledCalls;
uint8_t onVoltageRegulatorDisabledSequence;
uint8_t onMotorEnabledCalls;
uint8_t onMotorEnabledSequence;
uint8_t onMotorDisabledCalls;
uint8_t onMotorDisabledSequence;
uint8_t onMotorStartedCalls;
struct MotorStarted onMotorStartedArgs;
uint8_t onMotorStoppedCalls;
struct MotorStopped onMotorStoppedArgs;
uint16_t nearSchedulerAddCalls;
const struct NearSchedule *nearSchedulerAddArgs[8];

void motorFixtureSetUp(void)
{
	eventInitialise();

	callSequence = 1;
	pwmTimerEnableCalls = 0;
	pwmTimerDisableCalls = 0;
	voltageRegulatorDisableIsStubbedForEvent = 0;
	voltageRegulatorIsEnabledValue = 0;
	voltageRegulatorEnableCalls = 0;
	voltageRegulatorEnableSequence = 0;
	voltageRegulatorDisableCalls = 0;
	voltageRegulatorDisableSequence = 0;
	onVoltageRegulatorDisabledCalls = 0;
	onVoltageRegulatorDisabledSequence = 0;
	onMotorEnabledCalls = 0;
	onMotorEnabledSequence = 0;
	onMotorDisabledCalls = 0;
 	onMotorDisabledSequence = 0;
	onMotorStartedCalls = 0;
	onMotorStoppedCalls = 0;
	nearSchedulerAddCalls = 0;

	static const struct EventSubscription onMotorEnabledSubscription =
	{
		.type = MOTOR_ENABLED,
		.handler = &onMotorEnabled,
		.state = (void *) 0
	};

	eventSubscribe(&onMotorEnabledSubscription);

	static const struct EventSubscription onMotorDisabledSubscription =
	{
		.type = MOTOR_DISABLED,
		.handler = &onMotorDisabled,
		.state = (void *) 0
	};

	eventSubscribe(&onMotorDisabledSubscription);

	static const struct EventSubscription onMotorStartedSubscription =
	{
		.type = MOTOR_STARTED,
		.handler = &onMotorStarted,
		.state = (void *) 0
	};

	eventSubscribe(&onMotorStartedSubscription);

	static const struct EventSubscription onMotorStoppedSubscription =
	{
		.type = MOTOR_STOPPED,
		.handler = &onMotorStopped,
		.state = (void *) 0
	};

	eventSubscribe(&onMotorStoppedSubscription);
}

static void onMotorEnabled(const struct Event *event)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(event->args, "MTR EN");
	onMotorEnabledCalls++;
	onMotorEnabledSequence = callSequence++;
}

static void onMotorDisabled(const struct Event *event)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(event->args, "MTR DIS");
	onMotorDisabledCalls++;
	onMotorDisabledSequence = callSequence++;
}

void motorFixtureTearDown(void)
{
}

void pwmTimerEnable(void)
{
	pwmTimerEnableCalls++;
}

void pwmTimerDisable(void)
{
	pwmTimerDisableCalls++;
}

void ensureMotorFullyEnabled(void)
{
	stubVoltageRegulatorIsEnabled(1);
	publishVoltageRegulatorEnabled();
	motorEnable();
	dispatchAllEvents();
}

void stubVoltageRegulatorIsEnabled(uint8_t value)
{
	voltageRegulatorIsEnabledValue = value;
}

void publishVoltageRegulatorEnabled(void)
{
	static const struct VoltageRegulatorEnabled emptyEventArgs = { };
	eventPublish(VOLTAGE_REGULATOR_ENABLED, &emptyEventArgs);
}

void publishVoltageRegulatorDisabled(void)
{
	static const struct VoltageRegulatorDisabled emptyEventArgs = { };
	eventPublish(VOLTAGE_REGULATOR_DISABLED, &emptyEventArgs);
}

uint8_t voltageRegulatorIsEnabled(void)
{
	return voltageRegulatorIsEnabledValue;
}

void voltageRegulatorEnable(void)
{
	voltageRegulatorEnableCalls++;
	voltageRegulatorEnableSequence = callSequence++;
}

void voltageRegulatorDisable(void)
{
	voltageRegulatorDisableCalls++;
	voltageRegulatorDisableSequence = callSequence++;
	if (voltageRegulatorDisableIsStubbedForEvent)
		publishVoltageRegulatorDisabled();
}

void mockOnVoltageRegulatorDisabled(void)
{
	static const struct EventSubscription onVoltageRegulatorDisabledSubscription =
	{
		.type = VOLTAGE_REGULATOR_DISABLED,
		.handler = &onVoltageRegulatorDisabled,
		.state = (void *) 0
	};

	eventSubscribe(&onVoltageRegulatorDisabledSubscription);
}

static void onVoltageRegulatorDisabled(const struct Event *event)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(event->args, "VR DIS");
	onVoltageRegulatorDisabledCalls++;
	onVoltageRegulatorDisabledSequence = callSequence++;
}

void stubVoltageRegulatorDisableToPublishEvent(void)
{
	voltageRegulatorDisableIsStubbedForEvent = 1;
}

static void onMotorStarted(const struct Event *event)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(event->args, "MTR STD");
	onMotorStartedCalls++;
	memcpy(&onMotorStartedArgs, event->args, sizeof(struct MotorStarted));
}

static void onMotorStopped(const struct Event *event)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(event->args, "MTR SPD");
	onMotorStoppedCalls++;
	memcpy(&onMotorStoppedArgs, event->args, sizeof(struct MotorStopped));
}

int16_t anyClockwiseCount(void)
{
	return (int16_t) anyWordExcept(0) & 0x7fff;
}

int16_t anyAntiClockwiseCount(void)
{
	return -anyClockwiseCount();
}

int16_t anyEncoderCount(void)
{
	return (int16_t) anyWordExcept(0);
}

void nearSchedulerAdd(const struct NearSchedule *const schedule)
{
	nearSchedulerAddArgs[
		nearSchedulerAddCalls++ % (
			sizeof(nearSchedulerAddArgs) /
			sizeof(const struct NearSchedule *))
	] = schedule;
}

void publishWokenFromSleep(void)
{
	static const struct WokenFromSleep emptyEventArgs = { };
	eventPublish(WOKEN_FROM_SLEEP, &emptyEventArgs);
}
