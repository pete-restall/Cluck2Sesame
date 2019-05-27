#include <xc.h>

#include "../Event.h"
#include "../VoltageRegulator.h"

#include "Motor.h"

void motorEnable(void)
{
	voltageRegulatorEnable();

	if (motorState.enableCount++ == 0 && voltageRegulatorIsEnabled())
	{
		static const struct MotorEnabled eventArgs = { };
		eventPublish(MOTOR_ENABLED, &eventArgs);
		motorState.flags.isFullyEnabled = 1;
	}
}

void motorDisable(void)
{
	if (motorState.enableCount == 0)
		return;

	if (--motorState.enableCount == 0)
	{
		static const struct MotorDisabled eventArgs = { };
		eventPublish(MOTOR_DISABLED, &eventArgs);
		motorState.flags.isFullyEnabled = 0;
	}

	voltageRegulatorDisable();
}

uint8_t motorIsEnabled(void)
{
	return motorState.flags.isFullyEnabled != 0;
}
