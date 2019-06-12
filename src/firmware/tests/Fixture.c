#include <xc.h>
#include <stdint.h>

#include "Platform/Event.h"

#include "Fixture.h"

void setUp(void)
{
	onBeforeTest();
}

void tearDown(void)
{
	onAfterTest();
}

void dispatchAllEvents(void)
{
	while (eventDispatchNext())
		;;
}
