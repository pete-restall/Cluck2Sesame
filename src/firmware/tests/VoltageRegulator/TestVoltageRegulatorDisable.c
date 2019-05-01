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

void test_voltageRegulatorDisable_calledAfterEnabledOnce_expectEnablePinIsLow(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();
	voltageRegulatorDisable();
	TEST_ASSERT_FALSE(LATBbits.LATB2);
}

void test_voltageRegulatorDisable_calledOnceAfterEnabledTwice_expectEnablePinIsHigh(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();
	voltageRegulatorEnable();
	voltageRegulatorDisable();
	TEST_ASSERT_TRUE(LATBbits.LATB2);
}

void test_voltageRegulatorDisable_calledMoreTimesThanEnable_expectEnableCountDoesNotGetOutOfSync(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();
	voltageRegulatorDisable();
	voltageRegulatorDisable();
	voltageRegulatorDisable();
	voltageRegulatorEnable();
	TEST_ASSERT_TRUE_MESSAGE(LATBbits.LATB2, "EN");

	voltageRegulatorDisable();
	TEST_ASSERT_FALSE_MESSAGE(LATBbits.LATB2, "DIS");
}

void test_voltageRegulatorDisable_calledWhenAlreadyDisabled_expectNoEventIsPublished(void)
{
	voltageRegulatorInitialise();

	eventPublish_StubWithCallback(assertEventPublishNotCalled);
	voltageRegulatorDisable();
}

static void assertEventPublishNotCalled(
	EventType type,
	const void *const args,
	int numCalls)
{
	TEST_FAIL_MESSAGE("Expected eventPublish() not to be called");
}

void test_voltageRegulatorDisable_calledWhenStillEnabled_expectNoEventIsPublished(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();
	voltageRegulatorEnable();

	eventPublish_StubWithCallback(assertEventPublishNotCalled);
	voltageRegulatorDisable();
}

void test_voltageRegulatorDisable_calledWhenDisabled_expectVoltageRegulatorDisabledEventIsPublished(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();

	const struct VoltageRegulatorDisabled emptyArgs = { };
	eventPublish_Expect(VOLTAGE_REGULATOR_DISABLED, &emptyArgs);
	voltageRegulatorDisable();
}
