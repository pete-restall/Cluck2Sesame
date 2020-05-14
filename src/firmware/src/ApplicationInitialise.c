#include <xc.h>

#include "SunEvents.h"
#include "Door.h"
#include "Ui.h"

void applicationInitialise(void)
{
	sunEventsInitialise();
	doorInitialise();
	uiInitialise();
}
