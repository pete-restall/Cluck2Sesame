#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/Motor.h"

#include "MotorFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Motor/MotorInitialise.c")
TEST_FILE("Platform/Motor/MotorEnableDisable.c")
TEST_FILE("Platform/Motor/MotorOnOff.c")

static void wokenFromSleepWithShutdown(void);
static void wokenFromSleepWithoutShutdown(void);

void onBeforeTest(void)
{
	motorFixtureSetUp();
	motorInitialise();
}

void onAfterTest(void)
{
	motorFixtureTearDown();
}

void test_wokenFromSleep_onPublishedWithShutdownWhenNoFaults_expectMotorStoppedEventIsPublishedWithClearFaults(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR4bits.TMR1IF = 0;
	PIR2bits.C1IF = 0;
	wokenFromSleepWithShutdown();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedCalls, "Calls");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, onMotorStoppedArgs.fault.all, "Fault");
}

static void wokenFromSleepWithShutdown(void)
{
	PIR7bits.CWG1IF = 1;
	publishWokenFromSleep();
	dispatchAllEvents();
}

void test_wokenFromSleep_onPublishedWithShutdownWhenEncoderOverflowed_expectMotorStoppedEventIsPublishedWithEncoderOverflowFault(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR4bits.TMR1IF = 1;
	wokenFromSleepWithShutdown();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedCalls, "Calls");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedArgs.fault.encoderOverflow, "Fault");
}

void test_wokenFromSleep_onPublishedWithShutdownWhenEncoderOverflowed_expectTmr1InterruptFlagIsCleared(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR4bits.TMR1IF = 1;
	wokenFromSleepWithShutdown();
	TEST_ASSERT_FALSE(PIR4bits.TMR1IF);
}

void test_wokenFromSleep_onPublishedWithoutShutdownWhenEncoderOverflowed_expectMotorIsTurnedOff(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR4bits.TMR1IF = 1;
	PIR2bits.C1IF = 0;
	wokenFromSleepWithoutShutdown();
	TEST_ASSERT_EQUAL_UINT8(0, CWG1STR & ~STEERING_MASK);
}

static void wokenFromSleepWithoutShutdown(void)
{
	PIR7bits.CWG1IF = 0;
	publishWokenFromSleep();
	dispatchAllEvents();
}

void test_wokenFromSleep_onPublishedWithShutdownWhenCurrentLimited_expectMotorStoppedEventIsPublishedWithCurrentLimitedFault(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR2bits.C1IF = 1;
	PIR4bits.TMR1IF = 0;
	wokenFromSleepWithShutdown();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedCalls, "Calls");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedArgs.fault.currentLimited, "Fault");
}

void test_wokenFromSleep_onPublishedWithShutdownWhenCurrentLimited_expectCurrentSenseComparatorInterruptFlagIsCleared(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR2bits.C1IF = 1;
	wokenFromSleepWithShutdown();
	TEST_ASSERT_FALSE(PIR2bits.C1IF);
}

void test_wokenFromSleep_onPublishedWithoutShutdownWhenCurrentLimited_expectMotorIsTurnedOff(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR2bits.C1IF = 1;
	PIR4bits.TMR1IF = 0;
	wokenFromSleepWithoutShutdown();
	TEST_ASSERT_EQUAL_UINT8(0, CWG1STR & ~STEERING_MASK);
}

// TODO: motorOff() - PWM stops increasing (no more schedules)
// TODO: MOTOR_STOPPED when timeout - expect fault.encoderTimeout set
