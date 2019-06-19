#ifndef __CLUCK3SESAME_SRC_PLATFORM_PERIODICMONITOR_H
#define __CLUCK3SESAME_SRC_PLATFORM_PERIODICMONITOR_H
#include "Event.h"

#define MONITORED_PARAMETERS_SAMPLED ((EventType) 0x38)
struct MonitoredParametersSampled
{
	uint8_t whatever;
};

extern void periodicMonitorInitialise(void);

#endif
