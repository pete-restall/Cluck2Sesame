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

static void ensureMotorFullyEnabled(void);
static int16_t anyClockwiseCount(void);
static int16_t anyAntiClockwiseCount(void);
static int16_t anyEncoderCount(void);

void onBeforeTest(void)
{
	motorFixtureSetUp();
	motorInitialise();
}

void onAfterTest(void)
{
	motorFixtureTearDown();
}

void test_motorOn_calledWithPositiveCount_expectCcpLimitIsSameValue(void)
{
	ensureMotorFullyEnabled();
	int16_t clockwise = anyClockwiseCount();
	motorOn(clockwise);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE((clockwise >> 8) & 0xff, CCPR1H, "CCPR1H");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE((clockwise >> 0) & 0xff, CCPR1L, "CCPR1L");
}

static void ensureMotorFullyEnabled(void)
{
	stubVoltageRegulatorIsEnabled(1);
	motorEnable();
	dispatchAllEvents();
}

static int16_t anyClockwiseCount(void)
{
	return (int16_t) anyWordExcept(0) & 0x7fff;
}

void test_motorOn_calledWithNegativeCount_expectCcpLimitIsNegatedValue(void)
{
	ensureMotorFullyEnabled();
	int16_t antiClockwise = anyAntiClockwiseCount();
	motorOn(antiClockwise);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE((-antiClockwise >> 8) & 0xff, CCPR1H, "CCPR1H");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE((-antiClockwise >> 0) & 0xff, CCPR1L, "CCPR1L");
}

static int16_t anyAntiClockwiseCount(void)
{
	return -anyClockwiseCount();
}

void test_motorOn_calledWithMaximumNegativeCount_expectCcpLimitIsMaximumPositiveValue(void)
{
	ensureMotorFullyEnabled();
	motorOn(-32768);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x7f, CCPR1H, "CCPR1H");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0xff, CCPR1L, "CCPR1L");
}

void test_motorOn_called_expectTimer1IsCleared(void)
{
	ensureMotorFullyEnabled();
	TMR1H = anyByteExcept(0);
	TMR1L = anyByteExcept(0);
	motorOn(anyEncoderCount());
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, TMR1L, "TMR1L");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, TMR1H, "TMR1H");
}

static int16_t anyEncoderCount(void)
{
	return (int16_t) anyWordExcept(0);
}

void test_motorOn_called_expectTimer1InterruptFlagIsCleared(void)
{
	ensureMotorFullyEnabled();
	PIR4 = anyByteWithMaskSet(_PIR4_TMR1IF_MASK);
	uint8_t originalPir4 = PIR4;
	motorOn(anyEncoderCount());
	TEST_ASSERT_EQUAL_UINT8(originalPir4 & ~_PIR4_TMR1IF_MASK, PIR4);
}

void test_motorOn_calledWithZero_expectCcpLimitIsNotCleared(void)
{
	ensureMotorFullyEnabled();
	CCPR1H = anyByteExcept(0);
	CCPR1L = anyByteExcept(0);
	uint8_t originalCcpr1h = CCPR1H;
	uint8_t originalCcpr1l = CCPR1L;
	motorOn(0);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalCcpr1h, CCPR1H, "CCPR1H");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalCcpr1l, CCPR1L, "CCPR1L");
}

void test_motorOn_calledWithZero_expectTimer1IsNotCleared(void)
{
	ensureMotorFullyEnabled();
	TMR1H = anyByteExcept(0);
	TMR1L = anyByteExcept(0);
	uint8_t originalTmr1l = TMR1L;
	uint8_t originalTmr1h = TMR1H;
	motorOn(0);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalTmr1l, TMR1L, "TMR1L");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalTmr1h, TMR1H, "TMR1H");
}

void test_motorOn_calledWithZero_expectTimer1InterruptFlagIsNotCleared(void)
{
	ensureMotorFullyEnabled();
	PIR4bits.TMR1IF = 1;
	motorOn(0);
	TEST_ASSERT_TRUE(PIR4bits.TMR1IF);
}
