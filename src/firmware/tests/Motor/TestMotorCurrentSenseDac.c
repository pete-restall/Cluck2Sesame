#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Event.h"
#include "NvmSettings.h"
#include "Motor.h"

#include "MotorFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

TEST_FILE("Motor/MotorInitialise.c")

void onBeforeTest(void)
{
	motorFixtureSetUp();
	motorInitialise();
	PMD2bits.DAC1MD = 0;
}

void onAfterTest(void)
{
	motorFixtureTearDown();
}

void test_voltageRegulatorEnabled_onPublished_expectDacIsEnabled(void)
{
	DAC1CON0 = anyByteWithMaskClear(_DAC1CON0_EN_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_TRUE(DAC1CON0bits.EN);
}

void test_voltageRegulatorEnabled_onPublished_expectDacOutputPin1IsDisabled(void)
{
	DAC1CON0 = anyByteWithMaskSet(_DAC1CON0_DAC1OE1_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_FALSE(DAC1CON0bits.OE1);
}

void test_voltageRegulatorEnabled_onPublished_expectDacOutputPin2IsDisabled(void)
{
	DAC1CON0 = anyByteWithMaskSet(_DAC1CON0_DAC1OE2_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_FALSE(DAC1CON0bits.OE2);
}

void test_voltageRegulatorEnabled_onPublished_expectDacPositiveReferenceIsVdd(void)
{
	DAC1CON0 = anyByteWithMaskSet(
		_DAC1CON0_DAC1PSS0_MASK | _DAC1CON0_DAC1PSS1_MASK);

	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, DAC1CON0bits.PSS);
}

void test_voltageRegulatorEnabled_onPublished_expectDacNegativeReferenceIsVss(void)
{
	DAC1CON0 = anyByteWithMaskSet(_DAC1CON0_DAC1NSS_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_FALSE(DAC1CON0bits.NSS);
}

void test_voltageRegulatorEnabled_onPublished_expectCurrentLimitComesFromNvmSettings(void)
{
	DAC1CON1 = anyByteExcept(nvmSettings.motor.currentLimit);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(nvmSettings.motor.currentLimit, DAC1CON1);
}
