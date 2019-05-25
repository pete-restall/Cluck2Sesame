#include <xc.h>

#include "../Event.h"
#include "../PowerManagement.h"

#include "Clock.h"

#define T0CON0_POSTSCALE_1SECOND_MASK (0 << _T0CON0_T0OUTPS_POSITION)

#define T0CON1_SOSC_SOURCE_MASK (0b110 << _T0CON1_T0CS_POSITION)
#define T0CON1_PRESCALE_1SECOND_MASK (0b1111 << _T0CON1_T0CKPS_POSITION)

static void onWokenFromSleep(const struct Event *const event);

static void buggyCompilerWorkaround(void)
{
	static const struct DateChanged dummy1 = { .today = _OMNITARGET };
	static const struct TimeChanged dummy2 = { .now = _OMNITARGET };
}

void clockInitialise(void)
{
	PMD1bits.TMR0MD = 0;
	OSCENbits.SOSCEN = 1;
	while (!OSCSTATbits.SOR)
		;;

	T0CON0 = T0CON0_POSTSCALE_1SECOND_MASK;

	TMR0H = 59;
	TMR0L = 0;
	PIR0bits.TMR0IF = 0;
	PIE0bits.TMR0IE = 1;

	T0CON1 =
		_T0CON1_T0ASYNC_MASK |
		T0CON1_SOSC_SOURCE_MASK |
		T0CON1_PRESCALE_1SECOND_MASK;

	T0CON0bits.T0EN = 1;

	static const struct DateAndTimeSet epoch =
	{
		.date =
		{
			.year = 0,
			.month = 1,
			.day = 1
		}
	};

	clockSetNowGmt(&epoch);

	static const struct EventSubscription onWokenFromSleepSubscription =
	{
		.type = WOKEN_FROM_SLEEP,
		.handler = &onWokenFromSleep,
		.state = (void *) 0
	};

	eventSubscribe(&onWokenFromSleepSubscription);
}

static void onWokenFromSleep(const struct Event *const event)
{
	if (!PIR0bits.TMR0IF)
		return;

	PIR0bits.TMR0IF = 0;
	clockTicked();
}
