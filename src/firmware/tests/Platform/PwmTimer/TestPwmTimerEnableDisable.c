#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/PwmTimer.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/PwmTimer.c")

void onBeforeTest(void)
{
	pwmTimerInitialise();
}

void onAfterTest(void)
{
}

void test_pwmTimerEnable_calledOnce_expectTimer2IsEnabled(void)
{
	uint8_t originalT2con = T2CON;
	pwmTimerEnable();
	TEST_ASSERT_EQUAL_UINT8(T2CON | _T2CON_ON_MASK, T2CON);
}

void test_pwmTimerEnable_calledOnce_expectTimerInterruptFlagIsCleared(void)
{
	PIR4 = anyByteWithMaskSet(_PIR4_TMR2IF_MASK);
	uint8_t originalPir4 = PIR4;
	pwmTimerEnable();
	TEST_ASSERT_EQUAL_UINT8(originalPir4 & ~_PIR4_TMR2IF_MASK, PIR4);
}

void test_pwmTimerEnable_calledMoreThanOnceWhenInterruptFlagIsSet_expectTimerInterruptFlagIsNotCleared(void)
{
	pwmTimerEnable();

	PIR4 = anyByteWithMaskSet(_PIR4_TMR2IF_MASK);
	uint8_t originalPir4 = PIR4;
	pwmTimerEnable();
	TEST_ASSERT_EQUAL_UINT8(originalPir4 | _PIR4_TMR2IF_MASK, PIR4);
}

void test_pwmTimerDisable_calledWhenNotEnabled_expectTimer2IsDisabled(void)
{
	T2CON = anyByteWithMaskSet(_T2CON_ON_MASK);
	uint8_t originalT2con = T2CON;
	pwmTimerDisable();
	TEST_ASSERT_EQUAL_UINT8(T2CON & ~_T2CON_ON_MASK, T2CON);
}

void test_pwmTimerDisable_calledWhenNotEnabled_expectTimerInterruptFlagIsCleared(void)
{
	PIR4 = anyByteWithMaskSet(_PIR4_TMR2IF_MASK);
	uint8_t originalPir4 = PIR4;
	pwmTimerDisable();
	TEST_ASSERT_EQUAL_UINT8(originalPir4 & ~_PIR4_TMR2IF_MASK, PIR4);
}

void test_pwmTimerDisable_calledWhenEnabledOnce_expectTimer2IsDisabled(void)
{
	pwmTimerEnable();

	T2CON = anyByteWithMaskSet(_T2CON_ON_MASK);
	uint8_t originalT2con = T2CON;
	pwmTimerDisable();
	TEST_ASSERT_EQUAL_UINT8(T2CON & ~_T2CON_ON_MASK, T2CON);
}

void test_pwmTimerDisable_calledWhenEnabledOnce_expectTimerInterruptFlagIsCleared(void)
{
	pwmTimerEnable();

	PIR4 = anyByteWithMaskSet(_PIR4_TMR2IF_MASK);
	uint8_t originalPir4 = PIR4;
	pwmTimerDisable();
	TEST_ASSERT_EQUAL_UINT8(originalPir4 & ~_PIR4_TMR2IF_MASK, PIR4);
}

void test_pwmTimerDisable_calledLessTimesThanEnabled_expectTimerIsNotDisabled(void)
{
	pwmTimerEnable();
	pwmTimerEnable();

	uint8_t originalT2con = T2CON;
	pwmTimerDisable();
	TEST_ASSERT_EQUAL_UINT8(originalT2con, T2CON);
}

void test_pwmTimerDisable_calledLessTimesThanEnabled_expectTimerInterruptFlagNotCleared(void)
{
	pwmTimerEnable();
	pwmTimerEnable();

	PIR4 = anyByteWithMaskSet(_PIR4_TMR2IF_MASK);
	uint8_t originalPir4 = PIR4;
	pwmTimerDisable();
	TEST_ASSERT_EQUAL_UINT8(originalPir4, PIR4);
}

void test_pwmTimerDisable_calledMoreTimesThanEnabled_expectTimerCounterDoesNotGetOutOfSync(void)
{
	pwmTimerEnable();
	pwmTimerDisable();
	pwmTimerDisable();

	pwmTimerEnable();
	TEST_ASSERT_TRUE(T2CONbits.ON);
}
