#include <xc.h>

#include "Main.h"
#include "Event.h"
#include "Clock.h"
#include "NearScheduler.h"
#include "PowerManagement.h"
#include "VoltageRegulator.h"
#include "Lcd.h"
#include "SunEvents.h"

void initialise(void)
{
	static const struct SystemInitialised emptyEventArgs = { };
	eventInitialise();
	clockInitialise();
	nearSchedulerInitialise();
	powerManagementInitialise();
	voltageRegulatorInitialise();
	lcdInitialise();
	sunEventsInitialise();
	eventPublish(SYSTEM_INITIALISED, &emptyEventArgs);
}
