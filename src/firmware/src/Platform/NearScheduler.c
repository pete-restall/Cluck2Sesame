#include <xc.h>
#include <stdint.h>

#include "Event.h"
#include "PowerManagement.h"
#include "NearScheduler.h"

#define MAX_SCHEDULES 8
#define NCO_32768HZ_4MS_INCREMENT 8000
#define NCOCLK_SOURCE_SOSC (0b0101 << _NCO1CLK_N1CKS_POSITION)

static void buggyCompilerWorkaround(void)
{
	static const struct NearSchedule dummy =
	{
		.state = _OMNITARGET
	};
}

static void nearSchedulerAddTo(const struct NearSchedule *schedule, struct NearSchedule *ptr);
static void onWokenFromSleep(const struct Event *event);

static uint8_t ticks;
static struct NearSchedule schedules[MAX_SCHEDULES];
static struct NearSchedule *noMoreSchedules = schedules + MAX_SCHEDULES;

void nearSchedulerInitialise(void)
{
	PMD1bits.NCO1MD = 0;
	asm("nop");

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

void nearSchedulerAdd(const struct NearSchedule *schedule)
{
	struct NearSchedule *ptr;
	for (ptr = schedules; ptr != noMoreSchedules; ptr++)
	{
		if (!ptr->handler)
			break;
	}

	if (ptr == noMoreSchedules)
		return; // TODO: THIS SHOULD REGISTER A FAULT

	nearSchedulerAddTo(schedule, ptr);
}

static void nearSchedulerAddTo(const struct NearSchedule *schedule, struct NearSchedule *ptr)
{
	ptr->ticks = ticks + schedule->ticks + 1;
	ptr->handler = schedule->handler;
	ptr->state = schedule->state;
	if (!NCO1CONbits.N1EN)
	{
		if (schedule->ticks != 0)
			ptr->ticks--;

		NCO1ACCU = 0;
		NCO1ACCH = 0;
		NCO1ACCL = 0;
		NCO1CONbits.N1EN = 1;
	}
}

void nearSchedulerAddOrUpdate(const struct NearSchedule *schedule)
{
	struct NearSchedule *ptrUpdate;
	struct NearSchedule *ptrFree = (struct NearSchedule *) 0;
	for (ptrUpdate = schedules; ptrUpdate != noMoreSchedules; ptrUpdate++)
	{
		if (ptrUpdate->handler == schedule->handler)
			break;

		if (!ptrFree && !ptrUpdate->handler)
			ptrFree = ptrUpdate;
	}

	if (ptrUpdate == noMoreSchedules)
	{
		if (!ptrFree)
			return; // TODO: THIS SHOULD REGISTER A FAULT

		ptrUpdate = ptrFree;
	}

	nearSchedulerAddTo(schedule, ptrUpdate);
}

static void onWokenFromSleep(const struct Event *event)
{
	if (!PIR7bits.NCO1IF)
		return;

	PIR7bits.NCO1IF = 0;
	ticks++;
	uint8_t numberOfNonNullHandlers = 0;
	for (struct NearSchedule *ptr = schedules; ptr != noMoreSchedules; ptr++)
	{
		if (ptr->handler)
		{
			if (ptr->ticks == ticks)
			{
				NearScheduleHandler handler = ptr->handler;
				ptr->handler = (NearScheduleHandler) 0;
				handler(ptr->state);
			}

			if (ptr->handler)
				numberOfNonNullHandlers++;
		}
	}

	if (numberOfNonNullHandlers == 0)
		NCO1CONbits.N1EN = 0;
}
