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
	PMD2bits.CMP1MD = 0;
}

void onAfterTest(void)
{
	motorFixtureTearDown();
}

void test_voltageRegulatorEnabled_onPublished_expectComparatorIsEnabled(void)
{
	CM1CON0 = anyByteWithMaskClear(_CM1CON0_EN_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_TRUE(CM1CON0bits.EN);
}

void test_voltageRegulatorEnabled_onPublished_expectComparatorOutputIsInverted(void)
{
	CM1CON0 = anyByteWithMaskClear(_CM1CON0_POL_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_TRUE(CM1CON0bits.POL);
}

void test_voltageRegulatorEnabled_onPublished_expectComparatorHasHysteresis(void)
{
	CM1CON0 = anyByteWithMaskClear(_CM1CON0_HYS_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_TRUE(CM1CON0bits.HYS);
}

void test_voltageRegulatorEnabled_onPublished_expectComparatorIsAsynchronous(void)
{
	CM1CON0 = anyByteWithMaskSet(_CM1CON0_SYNC_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_FALSE(CM1CON0bits.SYNC);
}

void test_voltageRegulatorEnabled_onPublished_expectOnlyPositiveEdgeSetsInterruptFlag(void)
{
	CM1CON1 = anyByteWithMaskClear(_CM1CON1_INTP_MASK) | _CM1CON1_INTN_MASK;
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_TRUE_MESSAGE(CM1CON1bits.INTP, "INTP");
	TEST_ASSERT_FALSE_MESSAGE(CM1CON1bits.INTN, "INTN");
}

void test_voltageRegulatorEnabled_onPublished_expectComparatorInterruptFlagIsCleared(void)
{
	PIR2 = anyByteWithMaskSet(_PIR2_C1IF_MASK);
	uint8_t originalPir2 = PIR2;
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(originalPir2 & ~_PIR2_C1IF_MASK, PIR2);
}

void test_voltageRegulatorEnabled_onPublished_expectNegativeInputIsRb1(void)
{
	CM1NCH = anyByteExcept(3);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(3, CM1NCH);
}

void test_voltageRegulatorEnabled_onPublished_expectPositiveInputIsDac(void)
{
	CM1PCH = anyByteExcept(5);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(5, CM1PCH);
}
