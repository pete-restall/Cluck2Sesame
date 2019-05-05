#include <xc.h>
#include <unity.h>

#include "Event.h"
#include "Mock_VoltageRegulator.h"
#include "Mock_PwmTimer.h"
#include "Mock_LcdInternals.h"
#include "Lcd.h"

#include "NonDeterminism.h"

TEST_FILE("Event.c")
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

void test_lcdDisable_calledMoreTimesThanEnabled_expectVoltageRegulatorIsNotDisabledMoreTimesThanEnabled(void)
{
	voltageRegulatorDisable_Expect();
	lcdDisable();
	lcdDisable();
}

void test_lcdDisable_called_expectPwmTimerIsDisabled(void)
{
	pwmTimerDisable_Expect();
	lcdDisable();
}

void test_lcdDisable_calledMoreTimesThanEnabled_expectPwmTimerIsNotDisabledMoreTimesThanEnabled(void)
{
	pwmTimerDisable_Expect();
	lcdDisable();
	lcdDisable();
}

void test_lcdDisable_calledMoreTimesThanEnable_expectEnableCounterDoesNotGetOutOfSync(void)
{
	lcdDisable();
	lcdDisable();

	voltageRegulatorIsEnabled_ExpectAndReturn(1);
	lcdConfigure_Expect();
	lcdEnable();
}

// TODO: DISABLING THE LCD SHOULD TURN OFF THE DISPLAY IF THE VOLTAGE REGULATOR IS STILL ENABLED...
//
// TODO: DISABLING WHEN THE LCD IS BUSY (IE. WRITING COMMAND OR WAITING FOR COMMAND TO FINISH EXECUTING OR THE BIG 64ms WAIT DURING CONFIGURATION) - THE LCD SHOULD WAIT UNTIL NOT BUSY BEFORE TURNING OFF THE VOLTAGE REGULATOR...
