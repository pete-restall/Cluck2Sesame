#include <xc.h>
#include <stdint.h>

#include "Event.h"
#include "NvmSettings.h"
#include "PowerManagement.h"
#include "PeriodicMonitor.h"
#include "CalibrationMode.h"

#define PPS_IN_RB7 0x0f
#define PPS_OUT_CLKR 0x1b
#define PPS_OUT_UART1TX 0x10

#define CLKRCON_DUTY_CYCLE_50 (0b10 << _CLKRCON_CLKRDC_POSITION)
#define CLKRCON_NO_DIVIDER 0
#define CLKRCLK_SOURCE_SOSC (0b0101 << _CLKRCLK_CLKRCLK_POSITION)

static void onMonitoredParametersSampled(const struct Event *event);
static void onWokenFromSleep(const struct Event *event);

void calibrationModeInitialise(void)
{
	LATBbits.LATB6 = 0;
	LATBbits.LATB7 = 0;
	ANSELBbits.ANSB6 = 0;
	ANSELBbits.ANSB7 = 0;
	if (!nvmSettings.platform.flags.bits.isCalibrationRequired)
	{
		PMD4bits.UART1MD = 1;
		PMD0bits.CLKRMD = 1;
		ODCONBbits.ODCB6 = 0;
		RB6PPS = 0;
		RB7PPS = 0;
		RX1DTPPS = 0;
		TRISBbits.TRISB6 = 0;
		TRISBbits.TRISB7 = 0;
		return;
	}

	PMD0bits.CLKRMD = 0;
	ODCONBbits.ODCB6 = 1;
	TRISBbits.TRISB6 = 0;
	TRISBbits.TRISB7 = 1;
	CLKRCON = CLKRCON_DUTY_CYCLE_50 | CLKRCON_NO_DIVIDER;
	CLKRCLK = CLKRCLK_SOURCE_SOSC;
	RB6PPS = PPS_OUT_CLKR;

	PMD4bits.UART1MD = 0;
	RB7PPS = PPS_OUT_UART1TX;
	RX1DTPPS = PPS_IN_RB7;
	// TODO: ASYNC UART TX _AND_ RX ON RB7, NO CK (EXPLICIT SET TO PPS 0)...

	static const struct EventSubscription onMonitoredParametersSampledSubscription =
	{
		.type = MONITORED_PARAMETERS_SAMPLED,
		.handler = &onMonitoredParametersSampled,
		.state = (void *) 0
	};

	eventSubscribe(&onMonitoredParametersSampledSubscription);

	static const struct EventSubscription onWokenFromSleepSubscription =
	{
		.type = WOKEN_FROM_SLEEP,
		.handler = &onWokenFromSleep,
		.state = (void *) 0
	};

	eventSubscribe(&onWokenFromSleepSubscription);
}

static void onMonitoredParametersSampled(const struct Event *event)
{
	// TODO: THIS IS WHERE TEMPERATURE AND FVR MEASUREMENT EVENTS CAN BE HANDLED FROM...
	const struct MonitoredParametersSampled *args = (const struct MonitoredParametersSampled *) event->args;
	asm("nop");
}

static void onWokenFromSleep(const struct Event *event)
{
	// TODO: THIS IS WHERE UART1 EVENTS CAN BE HANDLED FROM... DON'T FORGET TO CHECK FOR RC1IF AND CLEAR RC1IF...
	const struct WokenFromSleep *args = (const struct WokenFromSleep *) event->args;
	asm("nop");
}
