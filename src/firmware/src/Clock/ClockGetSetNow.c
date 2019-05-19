#include <xc.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Clock.h"

static struct DateAndTimeGet clockNow;

void clockGetNowGmt(struct DateAndTimeGet *const now)
{
	clockNow.time.second = TMR0L;
	memcpy(now, &clockNow, sizeof(struct DateAndTimeGet));
}

void clockSetNowGmt(const struct DateAndTimeSet *const now)
{
	memcpy(&clockNow, now, sizeof(struct DateAndTimeSet));
	clockNow.date.flags.all = 0;
	clockNow.date.flags.isLeapYear = 1;
	TMR0L = clockNow.time.second;
	// TODO: SET isDaylightSavings, isLeapYear, dayOfYear CORRECTLY, ETC.
	// TODO: SET TMR0L WITH SECONDS
	// TODO: PUBLISH DATE_CHANGED EVENT
}

void clockTicked(void)
{
	clockNow.time.minute++;
}
