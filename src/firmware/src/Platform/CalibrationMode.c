#include <xc.h>
#include <stdint.h>

#include "Event.h"

#include "CalibrationMode.h"

void calibrationModeInitialise(void)
{
	// TODO: CLKR ON RB6
	// TODO: ASYNC UART TX _AND_ RX ON RB7, NO CK (EXPLICIT SET TO PPS 0)...
	// THE CRYSTAL WILL BE THE CLOCK, ALLOWING MEASUREMENT WITH A FREQUENCY COUNTER
}
