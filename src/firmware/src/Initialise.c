#include <xc.h>

#include "Main.h"
#include "Event.h"
#include "Clock.h"

static const struct SystemInitialised emptyEventArgs = { };

void initialise(void)
{
	clockInitialise();
	eventPublish(SYSTEM_INITIALISED, &emptyEventArgs);
}
