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
}

void onAfterTest(void)
{
	motorFixtureTearDown();
}

void test_voltageRegulatorDisabled_onPublished_expectCurrentSenseComparatorAndDacAreDisabled(void)
{
	static const uint8_t modules = _PMD2_CMP1MD_MASK | _PMD2_DAC1MD_MASK;
	PMD2 = anyByteWithMaskClear(modules);
	uint8_t originalPmd2 = PMD2;
	publishVoltageRegulatorDisabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(originalPmd2 | modules, PMD2);
}

void test_voltageRegulatorDisabled_onPublished_expectEncoderCcpAndMotorPwmModulesAreDisabled(void)
{
	static const uint8_t modules = _PMD3_CCP1MD_MASK | _PMD3_PWM4MD_MASK;
	PMD3 = anyByteWithMaskClear(modules);
	uint8_t originalPmd3 = PMD3;
	publishVoltageRegulatorDisabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(originalPmd3 | modules, PMD3);
}

void test_voltageRegulatorDisabled_onPublished_expectTimer1ModuleIsDisabled(void)
{
	PMD1 = anyByteWithMaskClear(_PMD1_TMR1MD_MASK);
	uint8_t originalPmd1 = PMD1;
	publishVoltageRegulatorDisabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(originalPmd1 | _PMD1_TMR1MD_MASK, PMD1);
}

void test_voltageRegulatorDisabled_onPublished_expectCwgModuleIsDisabled(void)
{
	PMD4 = anyByteWithMaskClear(_PMD4_CWG1MD_MASK);
	uint8_t originalPmd4 = PMD4;
	publishVoltageRegulatorDisabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(originalPmd4 | _PMD4_CWG1MD_MASK, PMD4);
}

void test_voltageRegulatorDisabled_onPublished_expectClc2ModuleIsDisabled(void)
{
	PMD5 = anyByteWithMaskClear(_PMD5_CLC2MD_MASK);
	uint8_t originalPmd5 = PMD5;
	publishVoltageRegulatorDisabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(originalPmd5 | _PMD5_CLC2MD_MASK, PMD5);
}
