#ifndef __CLUCK3SESAME_TESTS_PLATFORM_CLOCK_CLOCKFIXTURE_H
#define __CLUCK3SESAME_TESTS_PLATFORM_CLOCK_CLOCKFIXTURE_H
#include <stdint.h>
#include "Platform/Event.h"
#include "Platform/Clock.h"

extern void clockFixtureSetUp(void);
extern void clockFixtureTearDown(void);

extern void stubAnyDateTimeWithHourAndMinute(uint8_t hour, uint8_t minute);
extern void stubAnyDateTimeWithDayAndHourAndMinute(
	uint8_t day,
	uint8_t hour,
	uint8_t minute);

extern void stubAnyDateTime(void);

extern void assertEqualDateTime(
	const struct DateAndTimeGet *expected,
	const struct DateAndTimeGet *actual);

extern void assertEqualDateTimeExceptMinute(
	const struct DateAndTimeGet *expected,
	const struct DateAndTimeGet *actual);

extern void assertEqualDateTimeExceptHourAndMinute(
	const struct DateAndTimeGet *expected,
	const struct DateAndTimeGet *actual);

extern void assertEqualDateTimeExceptDayAndHourAndMinute(
	const struct DateAndTimeGet *expected,
	const struct DateAndTimeGet *actual);

extern void assertEqualDateTimeExceptMonthAndDayAndHourAndMinute(
	const struct DateAndTimeGet *expected,
	const struct DateAndTimeGet *actual);

extern void assertEqualDateTimeExceptYearAndMonthAndDayAndHourAndMinute(
	const struct DateAndTimeGet *expected,
	const struct DateAndTimeGet *actual);

extern void assertEqualDate(
	const struct Date *expected,
	const struct Date *actual);

extern void assertEqualTime(
	const struct Time *expected,
	const struct Time *actual);

extern void publishWokenFromSleep(void);
extern void mockOnTimeChanged(void);
extern void onTimeChanged(const struct Event *event);
extern void mockOnDateChanged(void);
extern void onDateChanged(const struct Event *event);

extern const struct DateChanged *dateChanged;
extern uint8_t dateChangedCalls;
extern uint8_t dateChangedSequence;

extern const struct TimeChanged *timeChanged;
extern uint8_t timeChangedCalls;
extern uint8_t timeChangedSequence;

#endif
