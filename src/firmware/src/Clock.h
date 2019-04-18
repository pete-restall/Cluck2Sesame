#ifndef __CLUCK3SESAME_SRC_CLOCK_H
#define __CLUCK3SESAME_SRC_CLOCK_H
#include "Event.h"

struct Date;

#define DATE_CHANGED ((EventType) 0x10)
struct DateChanged
{
	const struct Date *const today;
};

struct DateFlags
{
	unsigned int isLeapYear : 1;
	unsigned int isDaylightSavings : 1;
};

struct Date
{
	uint8_t day;
	uint8_t month;
	uint8_t year;
	uint16_t dayOfYear;
	struct DateFlags flags;
};

struct Time
{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
};

struct DateAndTime
{
	struct Time time;
	struct Date date;
};

extern void clockInitialise(void);

#endif
