#include <xc.h>
#include <unity.h>

#include "Mock_Event.h"
#include "Mock_NearScheduler.h"
#include "VoltageRegulator.h"

#include "NonDeterminism.h"

TEST_FILE("VoltageRegulator.c")

static void assertEventPublishNotCalled(
	EventType type,
	const void *const args,
	int numCalls);

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

void test_voltageRegulatorEnable_calledOnce_expectNoEventsPublished(void)
{
	voltageRegulatorInitialise();
	eventPublish_StubWithCallback(assertEventPublishNotCalled);
	voltageRegulatorEnable();
}

static void assertEventPublishNotCalled(
	EventType type,
	const void *const args,
	int numCalls)
{
	TEST_FAIL_MESSAGE("Expected eventPublish() not to be called");
}

void test_voltageRegulatorEnable_calledOnce_expectScheduleForRailStabilisationTime(void)
{
	voltageRegulatorInitialise();

	static const struct NearSchedule schedule =
	{
		.ticks = MS_TO_TICKS(64)
	};

	nearSchedulerAdd_Expect(&schedule);
	voltageRegulatorEnable();
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
