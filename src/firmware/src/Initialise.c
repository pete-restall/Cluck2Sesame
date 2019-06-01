#include <xc.h>

#include "Platform/Main.h"
#include "Platform/Event.h"
#include "Platform/Clock.h"
#include "Platform/NearScheduler.h"
#include "Platform/FarScheduler.h"
#include "Platform/PowerManagement.h"
#include "Platform/VoltageRegulator.h"
#include "Platform/PwmTimer.h"
#include "Platform/Lcd.h"
#include "Platform/Motor.h"

#include "SunEvents.h"

void initialise(void)
{
	static const struct SystemInitialised emptyEventArgs = { };
	eventInitialise();
	powerManagementInitialise();
	clockInitialise();
	nearSchedulerInitialise();
	farSchedulerInitialise();
	voltageRegulatorInitialise();
	pwmTimerInitialise();
	lcdInitialise();
	motorInitialise();
	sunEventsInitialise();
	eventPublish(SYSTEM_INITIALISED, &emptyEventArgs);
}
