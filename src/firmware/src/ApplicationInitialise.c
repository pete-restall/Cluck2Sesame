#include <xc.h>

#include "Location.h"
#include "SunEvents.h"
#include "Door.h"
#include "Ui.h"

void applicationInitialise(void)
{
	locationInitialise();
	sunEventsInitialise();
	doorInitialise();
	uiInitialise();
}
