#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Event.h"
#include "Motor.h"

#include "MotorFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

TEST_FILE("Motor/MotorInitialise.c")
TEST_FILE("Motor/MotorEnableDisable.c")
TEST_FILE("Motor/MotorOnOff.c")

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

void test_wokenFromSleep_onPublishedWithShutdownOfClockwiseTurn_expectCcpLimitIsSameValue(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	uint8_t originalCcpr1h = CCPR1H;
	uint8_t originalCcpr1l = CCPR1L;

	wokenFromSleepWithShutdown();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalCcpr1h, CCPR1H, "CCPR1H");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalCcpr1l, CCPR1L, "CCPR1L");
}

static void wokenFromSleepWithShutdown(void)
{
	PIR7bits.CWG1IF = 1;
	publishWokenFromSleep();
	dispatchAllEvents();
}

void test_wokenFromSleep_onPublishedWithShutdown_expectAllPwmOutputsAreDisabled(void)
{
	ensureMotorFullyEnabled();
	CWG1STR = anyByteWithMaskClear(STEERING_MASK);
	uint8_t originalCwg1strWithClearSteering = CWG1STR;
	motorOn(anyEncoderCount());

	wokenFromSleepWithShutdown();
	TEST_ASSERT_EQUAL_UINT8(originalCwg1strWithClearSteering, CWG1STR);
}

void test_wokenFromSleep_onPublishedWithoutShutdown_expectAllPwmOutputsAreUnchanged(void)
{
	ensureMotorFullyEnabled();
	CWG1STR = anyByteWithMaskClear(STEERING_MASK);
	motorOn(anyEncoderCount());
	uint8_t originalCwg1str = CWG1STR;

	PIR4bits.TMR1IF = 0;
	PIR2bits.C1IF = 0;
	wokenFromSleepWithoutShutdown();
	TEST_ASSERT_EQUAL_UINT8(originalCwg1str, CWG1STR);
}

static void wokenFromSleepWithoutShutdown(void)
{
	PIR7bits.CWG1IF = 0;
	publishWokenFromSleep();
	dispatchAllEvents();
}

void test_wokenFromSleep_onPublishedWithShutdown_expectShutdownInterruptFlagIsCleared(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR7 = anyByteWithMaskSet(_PIR7_CWG1IF_MASK);
	uint8_t originalPir7 = PIR7;
	wokenFromSleepWithShutdown();
	TEST_ASSERT_EQUAL_UINT8(originalPir7 & ~_PIR7_CWG1IF_MASK, PIR7);
}

void test_wokenFromSleep_onPublishedWithShutdownWhenMotorIsOff_expectMotorStoppedEventIsNotPublished(void)
{
	ensureMotorFullyEnabled();
	wokenFromSleepWithShutdown();
	TEST_ASSERT_EQUAL_UINT8(0, onMotorStoppedCalls);
}

void test_wokenFromSleep_onPublishedWithoutShutdown_expectMotorStoppedEventIsNotPublished(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR4bits.TMR1IF = 0;
	PIR2bits.C1IF = 0;
	wokenFromSleepWithoutShutdown();
	TEST_ASSERT_EQUAL_UINT8(0, onMotorStoppedCalls);
}

void test_wokenFromSleep_onPublishedWithoutShutdownButEncoderOverflowed_expectMotorStoppedEventIsPublished(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR4bits.TMR1IF = 1;
	PIR2bits.C1IF = 0;
	wokenFromSleepWithoutShutdown();
	TEST_ASSERT_EQUAL_UINT8(1, onMotorStoppedCalls);
}

void test_wokenFromSleep_onPublishedWithoutShutdownButCurrentLimited_expectMotorStoppedEventIsPublished(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR4bits.TMR1IF = 0;
	PIR2bits.C1IF = 1;
	wokenFromSleepWithoutShutdown();
	TEST_ASSERT_EQUAL_UINT8(1, onMotorStoppedCalls);
}

void test_wokenFromSleep_onPublishedWithShutdown_expectMotorStoppedEventIsPublishedWithSameRequestedCount(void)
{
	ensureMotorFullyEnabled();
	int16_t count = anyEncoderCount();
	motorOn(count);
	wokenFromSleepWithShutdown();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(count, onMotorStoppedArgs.requestedCount, "Count");
}

void test_wokenFromSleep_onPublishedWithShutdownWhenTurningClockwise_expectMotorStoppedEventIsPublishedWithTmr1AsActualCount(void)
{
	ensureMotorFullyEnabled();
	int16_t count = anyClockwiseCount();
	motorOn(count);

	TMR1H = (uint8_t) ((count >> 8) & 0xff);
	TMR1L = (uint8_t) ((count >> 0) & 0xff);

	wokenFromSleepWithShutdown();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(count, onMotorStoppedArgs.actualCount, "Count");
}

void test_wokenFromSleep_onPublishedWithShutdownWhenTurningAntiClockwise_expectMotorStoppedEventIsPublishedWithTmr1AsActualCount(void)
{
	ensureMotorFullyEnabled();
	int16_t count = anyAntiClockwiseCount();
	motorOn(count);

	int16_t negatedCount = -count;
	TMR1H = (uint8_t) ((negatedCount >> 8) & 0xff);
	TMR1L = (uint8_t) ((negatedCount >> 0) & 0xff);

	wokenFromSleepWithShutdown();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStoppedCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(count, onMotorStoppedArgs.actualCount, "Count");
}
