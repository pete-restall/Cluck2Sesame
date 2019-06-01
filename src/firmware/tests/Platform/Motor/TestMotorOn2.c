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

#define MIN(x, y) (x < y ? x : y)

static void fakeMotorAlreadyTurning(void);

void onBeforeTest(void)
{
	motorFixtureSetUp();
	motorInitialise();
}

void onAfterTest(void)
{
	motorFixtureTearDown();
}

void test_motorOn_calledWithClockwiseCount_expectPwmDutyCycleIsZero(void)
{
	ensureMotorFullyEnabled();
	PWM4DCH = anyByteExcept(0);
	PWM4DCL = anyByteExcept(0);
	motorOn(anyClockwiseCount());
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, PWM4DCL, "PWM4DCL");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, PWM4DCH, "PWM4DCH");
}

void test_motorOn_calledWithAntiClockwiseCount_expectPwmDutyCycleIsZero(void)
{
	ensureMotorFullyEnabled();
	PWM4DCH = anyByteExcept(0);
	PWM4DCL = anyByteExcept(0);
	motorOn(anyAntiClockwiseCount());
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, PWM4DCL, "PWM4DCL");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, PWM4DCH, "PWM4DCH");
}

void test_motorOn_calledWithZeroCount_expectPwmDutyCycleIsNotModified(void)
{
	ensureMotorFullyEnabled();
	PWM4DCH = anyByteExcept(0);
	PWM4DCL = anyByteExcept(0);
	uint8_t originalPwm4dcl = PWM4DCL;
	uint8_t originalPwm4dch = PWM4DCH;
	motorOn(0);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalPwm4dcl, PWM4DCL, "PWM4DCL");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalPwm4dch, PWM4DCH, "PWM4DCH");
}

void test_motorOn_calledWhenMotorAlreadyTurning_expectPwmDutyCycleIsNotModified(void)
{
	ensureMotorFullyEnabled();
	PWM4DCH = anyByteExcept(0);
	PWM4DCL = anyByteExcept(0);
	uint8_t originalPwm4dcl = PWM4DCL;
	uint8_t originalPwm4dch = PWM4DCH;

	fakeMotorAlreadyTurning();
	motorOn(anyEncoderCount());
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalPwm4dcl, PWM4DCL, "PWM4DCL");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalPwm4dch, PWM4DCH, "PWM4DCH");
}

static void fakeMotorAlreadyTurning(void)
{
	CWG1STR = anyByte();
	if (!(CWG1STR & STEERING_MASK))
		CWG1STR |= _CWG1STR_STRC_MASK;
}

void test_motorOn_calledWithZeroCount_expectMotorStartedEventIsNotPublished(void)
{
	ensureMotorFullyEnabled();
	motorOn(0);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, onMotorStartedCalls);
}

void test_motorOn_called_expectMotorStartedEventIsPublishedWithSameCount(void)
{
	ensureMotorFullyEnabled();
	int16_t count = anyEncoderCount();
	motorOn(count);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onMotorStartedCalls, "Calls");
	TEST_ASSERT_EQUAL_INT16_MESSAGE(count, onMotorStartedArgs.count, "Count");
}

void test_motorOn_called_expectNearScheduleAddedForPwm(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, nearSchedulerAddCalls, "Calls");
	TEST_ASSERT_NOT_NULL_MESSAGE(nearSchedulerAddArgs[0], "Args");
	TEST_ASSERT_NOT_NULL_MESSAGE(nearSchedulerAddArgs[0]->handler, "Handler");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, nearSchedulerAddArgs[0]->ticks, "Ticks");
}

void test_motorOn_calledWithZero_expectNoNearScheduleAddedForPwm(void)
{
	ensureMotorFullyEnabled();
	motorOn(0);
	TEST_ASSERT_EQUAL_UINT8(0, nearSchedulerAddCalls);
}

void test_motorOn_calledWhenAlreadyTurning_expectNoNearScheduleAddedForPwm(void)
{
	ensureMotorFullyEnabled();
	fakeMotorAlreadyTurning();
	motorOn(anyEncoderCount());
	TEST_ASSERT_EQUAL_UINT8(0, nearSchedulerAddCalls);
}

void test_motorOn_called_expectPwmIncrementsOnEachNearSchedulerTick(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());

	uint8_t i = 0;
	for (uint8_t pwmDutyCycle = 1; pwmDutyCycle != 0; pwmDutyCycle++)
	{
		const struct NearSchedule *schedule = nearSchedulerAddArgs[i++ & 0x07];
		if (schedule && schedule->handler)
			schedule->handler(schedule->state);

		uint8_t pwmLow = (pwmDutyCycle << 6) & 0xc0;
		uint8_t pwmHigh = (pwmDutyCycle >> 2) & 0x3f;
		TEST_ASSERT_EQUAL_UINT8_MESSAGE(pwmLow, PWM4DCL, "PWM4DCL");
		TEST_ASSERT_EQUAL_UINT8_MESSAGE(pwmHigh, PWM4DCH, "PWM4DCH");
	}
}

void test_motorOn_called_expectPwmIsNotIncrementedPast8BitMaximum(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());

	for (uint16_t calls = 0; calls < 256; calls++)
	{
		TEST_ASSERT_EQUAL_UINT16(MIN(255, 1 + calls), nearSchedulerAddCalls);

		const struct NearSchedule *schedule = nearSchedulerAddArgs[calls & 0x07];
		if (schedule && schedule->handler)
			schedule->handler(schedule->state);
	}
}

void test_motorOn_calledWhenMotorIsStoppedByInterruptBeforeFullPwmDutyCycle_expectPwmStopsIncrementing(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	PIR7bits.CWG1IF = 1;
	publishWokenFromSleep();
	dispatchAllEvents();

	const struct NearSchedule *schedule = nearSchedulerAddArgs[0];
	if (schedule && schedule->handler)
		schedule->handler(schedule->state);

	TEST_ASSERT_EQUAL_UINT16(1, nearSchedulerAddCalls);
}

void test_motorOn_calledWhenMotorIsStoppedManuallyBeforeFullPwmDutyCycle_expectPwmStopsIncrementing(void)
{
	ensureMotorFullyEnabled();
	motorOn(anyEncoderCount());
	motorOff();

	const struct NearSchedule *schedule = nearSchedulerAddArgs[0];
	if (schedule && schedule->handler)
		schedule->handler(schedule->state);

	TEST_ASSERT_EQUAL_UINT16(1, nearSchedulerAddCalls);
}
