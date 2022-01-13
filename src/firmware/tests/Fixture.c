#include <xc.h>
#include <stdint.h>

#include "Platform/Event.h"
#include "Platform/PowerManagement.h"

#include "Fixture.h"

const struct Event eventEmptyArgs __attribute__((weak)) = { };
volatile uint8_t sclDataOverlay[31] __section("sclDataOverlay");

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

void publishWokenFromSleep(void)
{
	eventPublish(WOKEN_FROM_SLEEP, &eventEmptyArgs);
}
