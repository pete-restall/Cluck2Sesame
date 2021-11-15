#include <xc.h>

#include "Main.h"
#include "Event.h"
#include "Clock.h"
#include "NearScheduler.h"
#include "FarScheduler.h"
#include "PowerManagement.h"
#include "VoltageRegulator.h"
#include "Battery.h"
#include "PwmTimer.h"
#include "Adc.h"
#include "Lcd.h"
#include "Motor.h"
#include "PeriodicMonitor.h"
#include "Buttons.h"

void initialise(void)
{
	eventInitialise();
	powerManagementInitialise();
	clockInitialise();
	nearSchedulerInitialise();
	farSchedulerInitialise();
	voltageRegulatorInitialise();
	batteryInitialise();
	pwmTimerInitialise();
	adcInitialise();
	lcdInitialise();
	motorInitialise();
	periodicMonitorInitialise();
	buttonsInitialise();
	applicationInitialise();
	eventPublish(SYSTEM_INITIALISED, &eventEmptyArgs);
}
