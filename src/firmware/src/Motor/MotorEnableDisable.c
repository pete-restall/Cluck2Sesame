#include <xc.h>

#include "../Event.h"
#include "../VoltageRegulator.h"

#include "Motor.h"

void motorEnable(void)
{
	voltageRegulatorEnable();

	if (motorEnableCount++ == 0 && voltageRegulatorIsEnabled())
	{
		static const struct MotorEnabled eventArgs = { };
		eventPublish(MOTOR_ENABLED, &eventArgs);
	}
}

void motorDisable(void)
{
	if (motorEnableCount == 0)
		return;

	if (--motorEnableCount == 0)
	{
		static const struct MotorDisabled eventArgs = { };
		eventPublish(MOTOR_DISABLED, &eventArgs);
	}

	voltageRegulatorDisable();
}
