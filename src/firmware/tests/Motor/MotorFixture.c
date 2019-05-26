#include <xc.h>
#include <unity.h>

#include "Event.h"
#include "Mock_VoltageRegulator.h"

#include "MotorFixture.h"

void publishVoltageRegulatorEnabled(void)
{
	static const struct VoltageRegulatorEnabled emptyEventArgs = { };
	eventPublish(VOLTAGE_REGULATOR_ENABLED, &emptyEventArgs);
}

void dispatchAllEvents(void)
{
	while (eventDispatchNext())
		;;
}

void publishVoltageRegulatorDisabled(void)
{
	static const struct VoltageRegulatorDisabled emptyEventArgs = { };
	eventPublish(VOLTAGE_REGULATOR_DISABLED, &emptyEventArgs);
}
