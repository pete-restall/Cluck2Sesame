#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/PowerManagement.h"
#include "Platform/Clock.h"

#include "ClockFixture.h"
#include "ClockGetSetNowFixture.h"

#include "../../NonDeterminism.h"
#include "../../Fixture.h"

void clockGetSetNowFixtureSetUp(void)
{
	eventInitialise();
	clockInitialise();
	T0CON0bits.T0EN = 0;
	T0CON1bits.T0CKPS = 0;

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(59, TMR0H, "60-second timebase required !");
}

void clockGetSetNowFixtureTearDown(void)
{
}

void tick(void)
{
	PIR0bits.TMR0IF = 1;
	publishWokenFromSleep();
	dispatchAllEvents();
}

uint8_t anyNonLeapYear(void)
{
	return anyNonLeapYearLessThan(100);
}

uint8_t anyNonLeapYearLessThan(uint8_t value)
{
	while (1)
	{
		uint8_t year = anyByteLessThan(value);
		if (year & 3)
			return year;
	};
}

uint8_t anyLeapYear(void)
{
	return anyByteLessThan(100) & ~3;
}
