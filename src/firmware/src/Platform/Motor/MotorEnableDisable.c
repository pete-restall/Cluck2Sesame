#include <xc.h>
#include <stdint.h>

#include "../Event.h"
#include "../VoltageRegulator.h"
#include "../PwmTimer.h"

#include "Motor.h"

void motorEnable(void)
{
	if (motorState.enableCount++ != 0)
		return;

	voltageRegulatorEnable();
	pwmTimerEnable();

	if (voltageRegulatorIsEnabled())
	{
		eventPublish(MOTOR_ENABLED, &eventEmptyArgs);
		motorState.flags.isFullyEnabled = 1;
	}
}

void motorDisable(void)
{
	if (motorState.enableCount == 0)
		return;

	if (--motorState.enableCount == 0)
	{
		eventPublish(MOTOR_DISABLED, &eventEmptyArgs);
		motorState.flags.isFullyEnabled = 0;

		pwmTimerDisable();
		voltageRegulatorDisable();
	}
}

uint8_t motorIsEnabled(void)
{
	return motorState.flags.isFullyEnabled != 0;
}
