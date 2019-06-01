#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Mock_VoltageRegulator.h"
#include "Mock_PwmTimer.h"
#include "Mock_LcdInternals.h"
#include "Platform/Lcd.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Event.c")
TEST_FILE("Platform/Lcd/LcdInitialise.c")
TEST_FILE("Platform/Lcd/LcdEnableDisable.c")

static void assertLcdConfigureNotCalled(int numCalls);
static void publishVoltageRegulatorEnabled(void);

void onBeforeTest(void)
{
	eventInitialise();
	lcdInitialise();
}

void onAfterTest(void)
{
}

void test_lcdEnable_calledWhenVoltageRegulatorIsNotEnabled_expectVoltageRegulatorIsEnabled(void)
{
	voltageRegulatorEnable_Expect();
	voltageRegulatorIsEnabled_ExpectAndReturn(0);
	lcdEnable();
}

void test_lcdEnable_calledWhenVoltageRegulatorIsEnabled_expectVoltageRegulatorIsEnabled(void)
{
	voltageRegulatorEnable_Expect();
	voltageRegulatorIsEnabled_ExpectAndReturn(1);
	lcdEnable();
}

void test_lcdEnable_called_expectPwmTimerIsEnabled(void)
{
	pwmTimerEnable_Expect();
	lcdEnable();
}

void test_lcdEnable_calledOnceWhenVoltageRegulatorIsEnabled_expectLcdIsConfigured(void)
{
	voltageRegulatorIsEnabled_ExpectAndReturn(1);
	lcdConfigure_Expect();
	lcdEnable();
}

void test_lcdEnable_calledMoreThanOnceWhenVoltageRegulatorIsEnabled_expectLcdIsOnlyConfiguredOnce(void)
{
	voltageRegulatorIsEnabled_ExpectAndReturn(1);
	lcdConfigure_Expect();
	lcdEnable();
	lcdEnable();
}

void test_lcdEnable_calledAfterDisabled_expectLcdIsConfigured(void)
{
	voltageRegulatorIsEnabled_ExpectAndReturn(1);
	lcdConfigure_Expect();
	lcdEnable();
	lcdDisable();

	voltageRegulatorIsEnabled_ExpectAndReturn(1);
	lcdConfigure_Expect();
	lcdEnable();
}

void test_lcdEnable_calledWhenVoltageRegulatorIsNotEnabled_expectLcdIsNotConfiguredUntilVoltageRegulatorEnabled(void)
{
	voltageRegulatorIsEnabled_ExpectAndReturn(0);
	lcdConfigure_StubWithCallback(assertLcdConfigureNotCalled);
	lcdEnable();
}

static void assertLcdConfigureNotCalled(int numCalls)
{
	TEST_FAIL_MESSAGE("Expected no calls to lcdConfigure()");
}

void test_lcdEnable_calledWhenVoltageRegulatorIsNotEnabledAndThenEventPublished_expectLcdIsConfigured(void)
{
	voltageRegulatorIsEnabled_ExpectAndReturn(0);
	lcdEnable();
	lcdConfigure_Expect();
	publishVoltageRegulatorEnabled();
	while (eventDispatchNext())
		;;
}

static void publishVoltageRegulatorEnabled(void)
{
	static const struct VoltageRegulatorEnabled emptyArgs = { };
	eventPublish(VOLTAGE_REGULATOR_ENABLED, &emptyArgs);
}

void test_lcdEnable_notCalled_expectVoltageRegulatorEnabledEventDoesNotConfigureLcd(void)
{
	lcdConfigure_StubWithCallback(assertLcdConfigureNotCalled);
	publishVoltageRegulatorEnabled();
	while (eventDispatchNext())
		;;
}
