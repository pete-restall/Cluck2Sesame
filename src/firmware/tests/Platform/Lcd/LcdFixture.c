#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/Clock.h"
#include "Platform/PowerManagement.h"
#include "Platform/NearScheduler.h"
#include "Platform/PwmTimer.h"
#include "Platform/Lcd.h"

#include "FakeLcd.h"
#include "LcdFixture.h"

#include "../../NonDeterminism.h"

extern void poll(void);

static void voltageRegulatorInitialise(void);
static void onLcdEnabled(const struct Event *event);
void voltageRegulatorDisable(void);

static uint8_t isLcdEnabled;

void lcdFixtureInitialise(void)
{
	eventInitialise();
	clockInitialise();
	powerManagementInitialise();
	nearSchedulerInitialise();
	pwmTimerInitialise();
	voltageRegulatorInitialise();
	lcdInitialise();
	fakeLcdInitialise();

	isLcdEnabled = 0;
}

static void voltageRegulatorInitialise(void)
{
	ANSELBbits.ANSB2 = 0;
	LATBbits.LATB2 = 0;
	TRISBbits.TRISB2 = 0;
}

void lcdFixtureShutdown(void)
{
	fakeLcdShutdown();
	voltageRegulatorDisable();
}

void enableLcdAndWaitUntilDone(void)
{
	static const struct EventSubscription onLcdEnabledSubscription =
	{
		.type = LCD_ENABLED,
		.handler = &onLcdEnabled,
		.state = (void *) 0
	};

	eventSubscribe(&onLcdEnabledSubscription);

	lcdEnable();
	while (!fakeLcdIsSessionInvalid && !isLcdEnabled)
		poll();
}

static void onLcdEnabled(const struct Event *event)
{
	isLcdEnabled = 1;
}

void voltageRegulatorEnable(void)
{
	LATBbits.LATB2 = 1;
}

void voltageRegulatorDisable(void)
{
	LATBbits.LATB2 = 0;
}

uint8_t voltageRegulatorIsEnabled(void)
{
	return LATBbits.LATB2 != 0;
}
