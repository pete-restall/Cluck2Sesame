#include <xc.h>
#include <stdint.h>

#include "Fixture.h"

void setUp(void)
{
	onBeforeTest();
}

void tearDown(void)
{
	onAfterTest();
}
