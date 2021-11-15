#ifndef __CLUCK3SESAME_SRC_PLATFORM_BATTERY_H
#define __CLUCK3SESAME_SRC_PLATFORM_BATTERY_H
#include "Event.h"

#define BATTERY_CHARGER_ENABLED ((EventType) 0x1c)
struct BatteryChargerEnabled { EMPTY_EVENT_ARGS };

#define BATTERY_CHARGER_DISABLED ((EventType) 0x1d)
struct BatteryChargerDisabled { EMPTY_EVENT_ARGS };

extern void batteryInitialise(void);

#endif
