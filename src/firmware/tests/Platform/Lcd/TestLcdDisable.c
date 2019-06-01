#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Mock_VoltageRegulator.h"
#include "Mock_PwmTimer.h"
#include "Mock_LcdInternals.h"
#include "Platform/Lcd.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

TEST_FILE("Platform/Event.c")
TEST_FILE("Platform/Lcd/LcdInitialise.c")
TEST_FILE("Platform/Lcd/LcdEnableDisable.c")

void onBeforeTest(void)
{
	lcdInitialise();
	lcdEnable();
}

void onAfterTest(void)
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
