#ifndef __CLUCK2SESAME_SRC_PLATFORM_TEMPERATURE_H
#define __CLUCK2SESAME_SRC_PLATFORM_TEMPERATURE_H
#include <stdint.h>
#include "Event.h"

#define TEMPERATURE_SAMPLED ((EventType) 0x3c)
struct TemperatureSampled
{
	uint16_t sample;
	int16_t currentCelsius;
	int16_t deltaCelsius;
	uint8_t deltaSeconds;
};

#define CELSIUS(celsius) ((int16_t) ((celsius) * 10))

extern void temperatureInitialise(void);

#endif
