#ifndef __CLUCK3SESAME_TESTS_CLOCK_CLOCKFIXTURE_H
#define __CLUCK3SESAME_TESTS_CLOCK_CLOCKFIXTURE_H
#include <stdint.h>
#include "Clock.h"

extern void stubAnyDateTimeWithMinute(uint8_t minute);
extern void stubAnyDateTimeWithHourAndMinute(uint8_t hour, uint8_t minute);
extern void stubAnyDateTimeWithDayAndHourAndMinute(
	uint8_t day,
	uint8_t hour,
	uint8_t minute);

extern void stubAnyDateTime(void);

extern void assertEqualDateTime(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual);

extern void assertEqualDateTimeExceptMinute(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual);

extern void assertEqualDateTimeExceptHourAndMinute(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual);

extern void assertEqualDateTimeExceptDayAndHourAndMinute(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual);

extern void assertEqualDateTimeExceptMonthAndDayAndHourAndMinute(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual);

extern void assertEqualDateTimeExceptYearAndMonthAndDayAndHourAndMinute(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual);

#endif
