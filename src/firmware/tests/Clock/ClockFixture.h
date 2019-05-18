#ifndef __CLUCK3SESAME_TESTS_CLOCK_CLOCKFIXTURE_H
#define __CLUCK3SESAME_TESTS_CLOCK_CLOCKFIXTURE_H
#include <stdint.h>
#include "Clock.h"

extern void stubAnyDateTimeWithSeconds(uint8_t seconds);
extern void stubAnyDateTimeWithMinutesAndSeconds(
	uint8_t minutes,
	uint8_t seconds);

extern void stubAnyDateTime(void);

extern void assertEqualDateTime(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual);

extern void assertEqualDateTimeExceptSecond(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual);

extern void assertEqualDateTimeExceptMinuteAndSecond(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual);

#endif
