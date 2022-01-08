#ifndef __CLUCK2SESAME_SRC_PLATFORM_BUTTONS_H
#define __CLUCK2SESAME_SRC_PLATFORM_BUTTONS_H
#include <stdint.h>
#include "Event.h"

#define BUTTONS_PRESSED ((EventType) 0x40)
struct ButtonsPressed
{
	uint8_t mask;
};

#define BUTTONS_RELEASED ((EventType) 0x41)
struct ButtonsReleased
{
	uint8_t mask;
};

extern void buttonsInitialise(void);

#endif
