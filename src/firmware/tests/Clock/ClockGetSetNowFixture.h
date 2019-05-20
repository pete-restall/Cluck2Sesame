#ifndef __CLUCK3SESAME_TESTS_CLOCK_CLOCKGETSETNOWFIXTURE_H
#define __CLUCK3SESAME_TESTS_CLOCK_CLOCKGETSETNOWFIXTURE_H
#include <stdint.h>

extern void clockGetSetNowFixtureSetUp(void);
extern void clockGetSetNowFixtureTearDown(void);

extern void tick(void);
extern uint8_t anyNonLeapYear(void);
extern uint8_t anyNonLeapYearLessThan(uint8_t value);
extern uint8_t anyLeapYear(void);

#endif
