#include <xc.h>

#include "SunEvents.h"
#include "Door.h"

void applicationInitialise(void)
{
	sunEventsInitialise();
	doorInitialise();
}
