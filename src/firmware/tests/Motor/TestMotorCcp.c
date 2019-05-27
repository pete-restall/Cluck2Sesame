#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Event.h"
#include "Mock_VoltageRegulator.h"
#include "Motor.h"

#include "MotorFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

TEST_FILE("Motor/MotorInitialise.c")

void onBeforeTest(void)
{
	eventInitialise();
	motorInitialise();
	PMD3bits.CCP1MD = 0;
}

void onAfterTest(void)
{
}

void test_voltageRegulatorEnabled_onPublished_expectCcpIsEnabled(void)
{
	CCP1CON = anyByteWithMaskClear(_CCP1CON_EN_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_TRUE(CCP1CONbits.EN);
}

void test_voltageRegulatorEnabled_onPublished_expectCcpModeIsCompareAndSetWithoutClearingTmr1(void)
{
	CCP1CON = anyByte();
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0b1000, CCP1CONbits.MODE);
}

void test_voltageRegulatorEnabled_onPublished_expectComparisonWordIsZero(void)
{
	CCPR1H = anyByteExcept(0);
	CCPR1L = anyByteExcept(0);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, CCPR1H, "CCPR1H");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, CCPR1L, "CCPR1L");
}
