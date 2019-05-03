#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Mock_Event.h"
#include "VoltageRegulator.h"

#include "VoltageRegulatorEnableFixture.h"
#include "NonDeterminism.h"

TEST_FILE("VoltageRegulator.c")
TEST_FILE("VoltageRegulatorEnableFixture.c")

void test_voltageRegulatorEnable_calledOnce_expectVoltageRegulatorEnabledIsPublishedWhenMcuRailHasStabilised(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();
	requestedSchedule->handler(requestedSchedule->state);

	static const struct VoltageRegulatorEnabled emptyArgs = { };
	eventPublish_Expect(VOLTAGE_REGULATOR_ENABLED, &emptyArgs);
	requestedSchedule->handler(requestedSchedule->state);
}

// TODO: TEST FOLLOWING SCENARIOS:
//
//
//     voltageRegulatorEnable();
//     voltageRegulatorDisable();
//     regulator startup time elapsed
//     EXPECT VMCU_SEL (RB0) IS LOW
//
//     voltageRegulatorEnable();
//     voltageRegulatorEnable();
//     voltageRegulatorDisable();
//     regulator startup time elapsed
//     EXPECT VMCU_SEL (RB0) IS HIGH
//
//     voltageRegulatorEnable();
//     regulator startup time elapsed
//     voltageRegulatorDisable();
//     EXPECT VMCU_SEL (RB0) IS LOW
//
//     voltageRegulatorEnable();
//     regulator startup time elapsed
//     voltageRegulatorEnable();
//     voltageRegulatorDisable();
//     EXPECT VMCU_SEL (RB0) IS HIGH
//
//     voltageRegulatorEnable();
//     regulator startup time elapsed
//     voltageRegulatorDisable();
//     EXPECT NO VOLTAGE_REGULATOR_ENABLED EVENT PUBLISHED !
//
//     voltageRegulatorEnable();
//     voltageRegulatorEnable();
//     EXPECT ONLY SINGLE VOLTAGE_REGULATOR_ENABLED EVENT PUBLISHED
