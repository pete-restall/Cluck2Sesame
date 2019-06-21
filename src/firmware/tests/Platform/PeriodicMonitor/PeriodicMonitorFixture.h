#ifndef __CLUCK3SESAME_TESTS_PLATFORM_PERIODICMONITOR_PERIODICMONITORFIXTURE_H
#define __CLUCK3SESAME_TESTS_PLATFORM_PERIODICMONITOR_PERIODICMONITORFIXTURE_H
#include <stdint.h>
#include "Platform/Event.h"
#include "Platform/Clock.h"
#include "Platform/Adc.h"
#include "Platform/PeriodicMonitor.h"

extern void periodicMonitorFixtureSetUp(void);
extern void periodicMonitorFixtureTearDown(void);

extern void mockOnMonitoredParametersSampled(void);
extern void publishTimeChanged(const struct Time *now);

extern void stubAdcSampleFor(struct AdcSample *sample);

extern const struct MonitoredParametersSampled *monitoredParametersSampled;
extern uint8_t monitoredParametersSampledCalls;
extern uint8_t monitoredParametersSampledSequence;

extern uint8_t adcSampleCalls;
extern uint8_t adcSampleSequence;

#endif
