#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Event.h"
#include "Motor.h"

#include "MotorFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

TEST_FILE("Motor/MotorInitialise.c")

void onBeforeTest(void)
{
	motorFixtureSetUp();
	motorInitialise();
	PMD1bits.TMR1MD = 0;
}

void onAfterTest(void)
{
	motorFixtureTearDown();
}

void test_voltageRegulatorEnabled_onPublished_expectTimer1IsEnabled(void)
{
	T1CON = anyByteWithMaskClear(_T1CON_ON_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_TRUE(T1CONbits.ON);
}

void test_voltageRegulatorEnabled_onPublished_expectTimer1PrescalerIs1To1(void)
{
	T1CON = anyByte();
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, T1CONbits.CKPS);
}

void test_voltageRegulatorEnabled_onPublished_expectTimer1IsSynchronousToAllowGlitchlessCompare(void)
{
	T1CON = anyByteWithMaskSet(_T1CON_nSYNC_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_FALSE(T1CONbits.nSYNC);
}

void test_voltageRegulatorEnabled_onPublished_expectTimer1ReadsAre16BitsBuffered(void)
{
	T1CON = anyByteWithMaskClear(_T1CON_RD16_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_TRUE(T1CONbits.RD16);
}

void test_voltageRegulatorEnabled_onPublished_expectTimer1CounterIsClear(void)
{
	TMR1H = anyByteExcept(0);
	TMR1L = anyByteExcept(0);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, TMR1L, "TMR1L");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, TMR1H, "TMR1H");
}

void test_voltageRegulatorEnabled_onPublished_expectTimer1GatingIsDisabled(void)
{
	T1GCON = anyByteWithMaskSet(_T1GCON_GE_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_FALSE(T1GCONbits.GE);
}

void test_voltageRegulatorEnabled_onPublished_expectTimer1SourceIsEncoderPin(void)
{
	T1CLK = anyByteExcept(0);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, T1CLK);
}

void test_voltageRegulatorEnabled_onPublished_expectEncoderAIsMappedToRc3(void)
{
	T1CKIPPS = anyByteExcept(0x13);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0x13, T1CKIPPS);
}

void test_voltageRegulatorEnabled_onPublished_expectTimer1InterruptFlagIsCleared(void)
{
	PIR4 = anyByteWithMaskSet(_PIR4_TMR1IF_MASK);
	uint8_t originalPir4 = PIR4;
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(originalPir4 & ~_PIR4_TMR1IF_MASK, PIR4);
}
