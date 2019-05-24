#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "../Event.h"

#include "Clock.h"

static uint8_t daysInMonth(uint8_t month, uint8_t year);

static struct DateAndTimeGet clockNow;

static const struct DateChanged dateChangedEventArgs =
{
	.today = &clockNow.date
};

static const struct TimeChanged timeChangedEventArgs =
{
	.now = &clockNow.time
};

void clockGetNowGmt(struct DateAndTimeGet *const now)
{
	clockNow.date.flags.isLeapYear = (clockNow.date.year & 3) == 0;
	clockNow.time.second = TMR0L;
	if (PIR0bits.TMR0IF)
	{
		TMR0IF = 0;
		clockTicked();
	// TODO: TEST FOR PUBLISH OF DATE_CHANGED EVENT, IF APPLICABLE (BEFORE TIME_CHANGED)
	// TODO: TEST FOR PUBLISH OF TIME_CHANGED EVENT (AFTER DATE_CHANGED)
	}

	memcpy(now, &clockNow, sizeof(struct DateAndTimeGet));
}

void clockTicked(void)
{
	uint8_t dateChanged = 0;

	TMR0H = 59;
	clockNow.time.second = TMR0L;
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

					clockNow.date.flags.isLeapYear =
						(clockNow.date.year & 3) == 0;

					clockNow.date.month = 1;
					clockNow.date.dayOfYear = UINT16_MAX;
				}

				clockNow.date.day = 0;
			}

			clockNow.date.day++;
			clockNow.date.dayOfYear++;
			clockNow.time.hour = 0;
			dateChanged = 1;
		}

		clockNow.time.minute = 0;
	}

	if (dateChanged)
		eventPublish(DATE_CHANGED, &dateChangedEventArgs);

	eventPublish(TIME_CHANGED, &timeChangedEventArgs);
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

void clockSetNowGmt(const struct DateAndTimeSet *const now)
{
	memcpy(&clockNow, now, sizeof(struct DateAndTimeSet));
	clockNow.date.flags.all = 0;
	clockNow.date.flags.isLeapYear = (clockNow.date.year & 3) == 0;
	TMR0H = 59;
	TMR0L = clockNow.time.second;
	PIR0bits.TMR0IF = 0;

	clockNow.date.dayOfYear = clockNow.date.day - 1;
	uint8_t month = clockNow.date.month;
	while (--month != 0)
		clockNow.date.dayOfYear += daysInMonth(month, clockNow.date.year);

	eventPublish(DATE_CHANGED, &dateChangedEventArgs);
	eventPublish(TIME_CHANGED, &timeChangedEventArgs);
}
