#ifndef __CLUCK3SESAME_SRC_PLATFORM_CLOCK_H
#define __CLUCK3SESAME_SRC_PLATFORM_CLOCK_H
#include <stdint.h>
#include "Event.h"

struct DateWithFlags;
struct Time;

#define DATE_CHANGED ((EventType) 0x10)
struct DateChanged
{
	const struct DateWithFlags *today;
};

#define TIME_CHANGED ((EventType) 0x11)
struct TimeChanged
{
	const struct Time *now;
};

struct Date
{
	uint8_t day;
	uint8_t month;
	uint8_t year;
};

struct DateWithFlags
{
	uint8_t day;
	uint8_t month;
	uint8_t year;
	uint16_t dayOfYear;

	union
	{
		uint8_t all;
		struct
		{
			unsigned int isLeapYear : 1;
			unsigned int isDaylightSavings : 1;
		};
	} flags;
};

struct Time
{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
};

struct DateAndTimeSet
{
	struct Time time;
	struct Date date;
};

struct DateAndTimeGet
{
	struct Time time;
	struct DateWithFlags date;
};

extern void clockInitialise(void);
extern void clockGetNowGmt(struct DateAndTimeGet *const now);
extern void clockSetNowGmt(const struct DateAndTimeSet *const now);

#endif
