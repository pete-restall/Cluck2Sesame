#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Mock_Event.h"
#include "VoltageRegulator.h"

#include "VoltageRegulatorFixture.h"
#include "NonDeterminism.h"

TEST_FILE("VoltageRegulator.c")
TEST_FILE("VoltageRegulatorFixture.c")

void test_voltageRegulatorIsEnabled_calledBeforeEnabled_expectFalse(void)
{
	voltageRegulatorInitialise();
	TEST_ASSERT_FALSE(voltageRegulatorIsEnabled());
}

void test_voltageRegulatorIsEnabled_calledImmediatelyAfterEnable_expectFalse(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();
	TEST_ASSERT_FALSE(voltageRegulatorIsEnabled());
}

void test_voltageRegulatorIsEnabled_calledWhenVoltageRailHasStabilised_expectFalse(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();
	callScheduleHandlerAndForget();
	TEST_ASSERT_FALSE(voltageRegulatorIsEnabled());
}

void test_voltageRegulatorIsEnabled_calledWhenFullyEnabled_expectTrue(void)
{
	voltageRegulatorInitialise();
	fullyEnableVoltageRegulator();
	TEST_ASSERT_TRUE(voltageRegulatorIsEnabled());
}

void test_voltageRegulatorIsEnabled_calledWhenFullyEnabledMoreTimesThanDisabled_expectTrue(void)
{
	voltageRegulatorInitialise();
	fullyEnableVoltageRegulator();
	fullyEnableVoltageRegulatorWithoutAssertions();
	voltageRegulatorDisable();
	TEST_ASSERT_TRUE(voltageRegulatorIsEnabled());
}

void test_voltageRegulatorIsEnabled_calledWhenDisabledAfterBeingFullyEnabled_expectFalse(void)
{
	voltageRegulatorInitialise();
	fullyEnableVoltageRegulator();
	voltageRegulatorDisable();
	TEST_ASSERT_FALSE(voltageRegulatorIsEnabled());
}

void test_voltageRegulatorIsEnabled_calledWhenDisabledBeforeBeingFullyEnabled_expectFalse(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();
	callScheduleHandlerAndForget();
	voltageRegulatorDisable();
	callScheduleHandlerAndForget();
	TEST_ASSERT_FALSE(voltageRegulatorIsEnabled());
}
