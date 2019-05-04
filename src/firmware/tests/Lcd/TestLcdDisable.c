#include <xc.h>
#include <unity.h>

#include "Mock_VoltageRegulator.h"
#include "Mock_PwmTimer.h"
#include "Lcd.h"

#include "NonDeterminism.h"

TEST_FILE("Lcd/LcdInitialise.c")
TEST_FILE("Lcd/LcdEnableDisable.c")

void setUp(void)
{
	lcdInitialise();
	lcdEnable();
}

void tearDown(void)
{
}

void test_lcdDisable_called_expectVoltageRegulatorIsDisabled(void)
{
	voltageRegulatorDisable_Expect();
	lcdDisable();
}

void test_lcdDisable_called_expectPwmTimerIsDisabled(void)
{
	pwmTimerDisable_Expect();
	lcdDisable();
}
