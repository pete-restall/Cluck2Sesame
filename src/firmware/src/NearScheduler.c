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

	for (uint8_t i = 0; i < MAX_SCHEDULES; i++)
		schedules[i].handler = (NearScheduleHandler) 0;
}

void nearSchedulerAdd(const struct NearSchedule *const schedule)
{
	struct NearSchedule *ptr = (struct NearSchedule *) 0;
	for (uint8_t i = 0; i < MAX_SCHEDULES; i++)
	{
		if (!schedules[i].handler)
		{
			ptr = &schedules[i];
			break;
		}
	}

	// TODO: NULL CHECK, IE. NO SPACE LEFT FOR HANDLERS !

	ptr->handler = schedule->handler;
	ptr->state = schedule->state;
	if (!NCO1CONbits.N1EN)
	{
		ptr->ticks = ticks + (schedule->ticks != 0 ? schedule->ticks : 1);

		NCO1ACCU = 0;
		NCO1ACCH = 0;
		NCO1ACCL = 0;
		NCO1CONbits.N1EN = 1;
	}
	else
		ptr->ticks = ticks + (schedule->ticks != 0 ? schedule->ticks + 1 : 1);
}

static void onWokenFromSleep(const struct Event *const event)
{
	if (!PIR7bits.NCO1IF)
		return;

	PIR7bits.NCO1IF = 0;
	ticks++;
	for (uint8_t i = 0; i < MAX_SCHEDULES; i++)
	{
		if (schedules[i].handler && schedules[i].ticks == ticks)
			schedules[i].handler(&schedules[i]);
	}
}
