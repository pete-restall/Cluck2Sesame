#include <xc.h>

#include "Main.h"

#ifndef TEST

void main(void)
{
	initialise();
	while (1)
		poll();
}

#endif
