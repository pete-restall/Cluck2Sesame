#include <xc.h>
#include <unity.h>

#include "Mock_Event.h"
#include "Mock_VoltageRegulator.h"
#include "Mock_PwmTimer.h"
#include "Lcd.h"

#include "NonDeterminism.h"

TEST_FILE("Lcd/LcdInitialise.c")
TEST_FILE("Lcd/LcdEnableDisable.c")

void setUp(void)
{
	lcdInitialise();
}

void tearDown(void)
{
}

void test_lcdEnable_called_expectVoltageRegulatorIsEnabled(void)
{
	voltageRegulatorEnable_Expect();
	lcdEnable();
}

void test_lcdEnable_called_expectPwmTimerIsEnabled(void)
{
	pwmTimerEnable_Expect();
	lcdEnable();
}
