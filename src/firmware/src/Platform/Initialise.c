#include <xc.h>

#include "Main.h"
#include "Event.h"
#include "Clock.h"
#include "NearScheduler.h"
#include "FarScheduler.h"
#include "PowerManagement.h"
#include "VoltageRegulator.h"
#include "PwmTimer.h"
#include "Adc.h"
#include "Lcd.h"
#include "Motor.h"

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
	adcInitialise();
	lcdInitialise();
	motorInitialise();
	applicationInitialise();
	eventPublish(SYSTEM_INITIALISED, &emptyEventArgs);
}
