#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Mock_Event.h"
#include "VoltageRegulator.h"

#include "VoltageRegulatorFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

TEST_FILE("VoltageRegulator.c")
TEST_FILE("VoltageRegulatorFixture.c")

void test_voltageRegulatorEnable_calledOnce_expectVoltageRegulatorEnabledIsPublishedWhenMcuRailHasStabilised(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();
	callScheduleHandlerAndForget();

	static const struct VoltageRegulatorEnabled emptyArgs = { };
	eventPublish_Expect(VOLTAGE_REGULATOR_ENABLED, &emptyArgs);
	callScheduleHandlerAndForget();
}

void test_voltageRegulatorEnable_calledTwice_expectNoNewScheduleIsAdded(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();
	requestedSchedule = (struct NearSchedule *) 0;
	voltageRegulatorEnable();
	TEST_ASSERT_NULL(requestedSchedule);
}

void test_voltageRegulatorEnable_calledAfterVoltageRailHasStabilised_expectNoNewScheduleIsAdded(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();
	callScheduleHandlerAndForget();
	requestedSchedule = (struct NearSchedule *) 0;
	voltageRegulatorEnable();
	TEST_ASSERT_NULL(requestedSchedule);
}

void test_voltageRegulatorEnable_calledAfterVoltageRailHasStabilised_expectOnlyOneEventIsPublished(void)
{
	static const struct VoltageRegulatorEnabled emptyArgs = { };
	eventPublish_Expect(VOLTAGE_REGULATOR_ENABLED, &emptyArgs);

	voltageRegulatorInitialise();
	voltageRegulatorEnable();
	callScheduleHandlerAndForget();
	voltageRegulatorEnable();
	callScheduleHandlerAndForget();
}

void test_voltageRegulatorEnable_calledAfterFullyEnabled_expectNoNewScheduleIsAdded(void)
{
	voltageRegulatorInitialise();
	fullyEnableVoltageRegulator();
	voltageRegulatorEnable();
	TEST_ASSERT_NULL(requestedSchedule);
}
