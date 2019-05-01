#include <xc.h>
#include <stdint.h>

#include "Event.h"
#include "PowerManagement.h"
#include "NearScheduler.h"

#define MAX_SCHEDULES 8
#define NCO_32768HZ_4MS_INCREMENT 8000
#define NCOCLK_SOURCE_SOSC (0b0101 << _NCO1CLK_N1CKS_POSITION)

static void onWokenFromSleep(const struct Event *const event);

static uint8_t ticks;
static struct NearSchedule schedules[MAX_SCHEDULES];

void nearSchedulerInitialise(void)
{
	NCO1CON = 0;
	NCO1CLK = NCOCLK_SOURCE_SOSC;
	NCO1INCU = 0;
	NCO1INCH = (uint8_t) ((NCO_32768HZ_4MS_INCREMENT >> 8) & 0xff);
	NCO1INCL = (uint8_t) ((NCO_32768HZ_4MS_INCREMENT >> 0) & 0xff);
	PIR7bits.NCO1IF = 0;
	PIE7bits.NCO1IE = 1;

	static const struct EventSubscription onWokenFromSleepSubscription =
	{
		.type = WOKEN_FROM_SLEEP,
		.handler = &onWokenFromSleep,
		.state = (void *) 0
	};

	eventSubscribe(&onWokenFromSleepSubscription);
}

void nearSchedulerAdd(const struct NearSchedule *const schedule)
{
	schedules[0].ticks = ticks + (schedule->ticks != 0 ? schedule->ticks : 1);
	schedules[0].handler = schedule->handler;
	schedules[0].state = schedule->state;
	if (!NCO1CONbits.N1EN)
	{
		NCO1ACCU = 0;
		NCO1ACCH = 0;
		NCO1ACCL = 0;
		NCO1CONbits.N1EN = 1;
	}
}

static void onWokenFromSleep(const struct Event *const event)
{
	if (!PIR7bits.NCO1IF)
		return;

	PIR7bits.NCO1IF = 0;
	if (++ticks == schedules[0].ticks && schedules[0].handler)
		schedules[0].handler(&schedules[0]);
}
