#include <xc.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Clock.h"

static struct DateAndTimeGet clockNow;

void clockGetNowGmt(struct DateAndTimeGet *const now)
{
	// TODO: EVEN WHEN THE CLOCK HAS NOT TICKED, WE STILL NEED THE CORRECT
	//       TIME (IE. IT MAY BE TICKING AT 256-SECOND INTERVALS)
	memcpy(now, &clockNow, sizeof(struct DateAndTimeGet));
}

void clockSetNowGmt(const struct DateAndTimeSet *const now)
{
	memcpy(&clockNow, now, sizeof(struct DateAndTimeSet));
	clockNow.date.flags.all = 0;
	clockNow.date.flags.isLeapYear = 1;
	// TODO: SET isDaylightSavings, isLeapYear, dayOfYear CORRECTLY, ETC.
	// TODO: PUBLISH DATE_CHANGED EVENT
}

void clockTicked(void)
{
	int seconds = clockNow.time.second + (TMR0H == 0 ? 256 : TMR0H);
	if (seconds >= 60)
	{
		div_t min_sec = div(seconds, 60);
		clockNow.time.second = (uint8_t) min_sec.rem;
		clockNow.time.minute += (uint8_t) min_sec.quot;
	}
	else
		clockNow.time.second = (uint8_t) seconds;
}
