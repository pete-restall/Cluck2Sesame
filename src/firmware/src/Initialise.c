#include <xc.h>

#include "Main.h"
#include "Event.h"
#include "Clock.h"
#include "PowerManagement.h"

void initialise(void)
{
	static const struct SystemInitialised emptyEventArgs = { };
	eventInitialise();
	clockInitialise();
	powerManagementInitialise();
	eventPublish(SYSTEM_INITIALISED, &emptyEventArgs);
}
