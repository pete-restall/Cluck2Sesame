#ifndef __CLUCK3SESAME_SRC_PLATFORM_PERIODICMONITOR_H
#define __CLUCK3SESAME_SRC_PLATFORM_PERIODICMONITOR_H
#include "Event.h"

#define MONITORED_PARAMETERS_SAMPLED ((EventType) 0x38)
struct MonitoredParametersSampled
{
	uint8_t timestamp;
	union
	{
		uint8_t all;
		struct
		{
			unsigned int isVddRegulated : 1;
		};
	} flags;
	uint16_t fvr;
	uint16_t temperature;
};

extern void periodicMonitorInitialise(void);

#ifdef __PERIODICMONITOR_EXPOSE_INTERNALS
extern void periodicMonitorSampleNow(struct MonitoredParametersSampled *eventArgs);
#endif

#endif
