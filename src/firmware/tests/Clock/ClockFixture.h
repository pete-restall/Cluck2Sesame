#ifndef __CLUCK3SESAME_TESTS_CLOCK_CLOCKFIXTURE_H
#define __CLUCK3SESAME_TESTS_CLOCK_CLOCKFIXTURE_H
#include <stdint.h>
#include "Clock.h"

extern void stubAnyDateTimeWithMinutes(uint8_t minutes);
extern void stubAnyDateTime(void);

extern void assertEqualDateTime(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual);

extern void assertEqualDateTimeExceptMinute(
	const struct DateAndTimeGet *const expected,
	const struct DateAndTimeGet *const actual);

#endif
