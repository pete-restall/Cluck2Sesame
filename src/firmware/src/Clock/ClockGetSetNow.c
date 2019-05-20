#include <xc.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Clock.h"

static uint8_t daysInMonth(uint8_t month, uint8_t year);

static struct DateAndTimeGet clockNow;

void clockGetNowGmt(struct DateAndTimeGet *const now)
{
	// TODO: IF THERE IS A TMR0IF SOMEWHERE IN HERE THEN WE RISK GETTING A
	// CORRUPTED DATE / TIME (IE. 23:59 TICKS OVER TO THE NEXT DAY, OR TIME
	// APPEARING TO GO BACKWARDS IF JUST THE 59TH MINUTE TICKS OVER AND THE
	// HOUR IS UNCHANGED)
	clockNow.date.flags.isLeapYear = (clockNow.date.year & 3) == 0;
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
	// TODO: PUBLISH DATE_CHANGED EVENT, IF APPLICABLE (BEFORE TIME_CHANGED)
	// TODO: PUBLISH TIME_CHANGED EVENT (AFTER DATE_CHANGED)

	clockNow.date.dayOfYear = clockNow.date.day - 1;
	uint8_t month = clockNow.date.month;
	while (--month != 0)
		clockNow.date.dayOfYear += daysInMonth(month, clockNow.date.year);
}

static uint8_t daysInMonth(uint8_t month, uint8_t year)
{
	if (month == 2)
	{
		if ((year & 3) == 0)
			return 29;

		return 28;
	}

	uint8_t mask30Days = (month & 0b1001);
	if (mask30Days == 0b1001 || mask30Days == 0b0000)
		return 30;

	return 31;
}

void clockTicked(void)
{
	if (++clockNow.time.minute == 60)
	{
		if (++clockNow.time.hour == 24)
		{
			uint8_t lastDayOfMonth = daysInMonth(
				clockNow.date.month,
				clockNow.date.year);

			if (clockNow.date.day == lastDayOfMonth)
			{
				if (clockNow.date.month++ == 12)
				{
					if (++clockNow.date.year == 100)
						clockNow.date.year = 0;

					clockNow.date.month = 1;
					clockNow.date.dayOfYear = UINT16_MAX;
				}

				clockNow.date.day = 0;
			}

			clockNow.date.day++;
			clockNow.date.dayOfYear++;
			clockNow.time.hour = 0;
		}

		clockNow.time.minute = 0;
	}
	// TODO: PUBLISH DATE CHANGED (IF APPLICABLE).  MAKE SURE IT'S BEFORE TIME_CHANGED
	// TODO: PUBLISH TIME_CHANGED.  MAKE SURE IT'S AFTER DATE_CHANGED
}
