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
}

void onAfterTest(void)
{
}

void test_voltageRegulatorDisabled_onPublished_expectEncoderPinsAreOutputs(void)
{
	static const uint8_t encoderPins = _TRISC_TRISC2_MASK | _TRISC_TRISC3_MASK;
	TRISC = anyByteWithMaskSet(encoderPins);
	uint8_t originalTrisc = TRISC;
	publishVoltageRegulatorDisabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(originalTrisc & ~encoderPins, TRISC);
}

void test_voltageRegulatorDisabled_onPublished_expectCurrentSensePinIsOutput(void)
{
	TRISB = anyByteWithMaskSet(_TRISB_TRISB1_MASK);
	uint8_t originalTrisb = TRISB;
	publishVoltageRegulatorDisabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(originalTrisb & ~_TRISB_TRISB1_MASK, TRISB);
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
