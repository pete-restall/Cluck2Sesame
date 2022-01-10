#ifndef __CLUCK2SESAME_SRC_PLATFORM_BATTERY_H
#define __CLUCK2SESAME_SRC_PLATFORM_BATTERY_H
#include <stdint.h>
#include "Event.h"

#define BATTERY_VOLTAGE_SAMPLED ((EventType) 0x1c)
struct BatteryVoltageSampled
{
	uint16_t sample;
	uint16_t millivolts;
};

#define BATTERY_CHARGER_ENABLED ((EventType) 0x1d)
struct BatteryChargerEnabled { EMPTY_EVENT_ARGS };

#define BATTERY_CHARGER_DISABLED ((EventType) 0x1e)
struct BatteryChargerDisabled { EMPTY_EVENT_ARGS };

extern void batteryInitialise(void);

#endif
