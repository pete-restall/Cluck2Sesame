#include <xc.h>

#include "Clock.h"

void clockInitialise(void)
{
	OSCENbits.SOSCEN = 1;
	while (!OSCSTATbits.SOR)
		;;
}
