#include <xc.h>
#include <unity.h>

#include "Mock_Event.h"
#include "VoltageRegulator.h"

#include "NonDeterminism.h"

TEST_FILE("VoltageRegulator.c")

void setUp(void)
{
}

void tearDown(void)
{
}

void test_voltageRegulatorEnable_calledOnce_expectEnablePinIsHigh(void)
{
	LATB = anyByte();
	uint8_t originalLatb = LATB;
	voltageRegulatorInitialise();

	voltageRegulatorEnable();
	TEST_ASSERT_EQUAL_UINT8(originalLatb | _LATB_LATB2_MASK, LATB);
}

// TODO: TEST FOLLOWING SCENARIOS:
//     voltageRegulatorEnable();
//     startup time elapsed
//     EXPECT VOLTAGE_REGULATOR_ENABLED EVENT PUBLISHED
//
//     voltageRegulatorEnable();
//     voltageRegulatorDisable();
//     startup time elapsed
//     EXPECT NO VOLTAGE_REGULATOR_ENABLED EVENT PUBLISHED !
//
//     voltageRegulatorEnable();
//     voltageRegulatorEnable();
//     EXPECT ONLY SINGLE VOLTAGE_REGULATOR_ENABLED EVENT PUBLISHED
