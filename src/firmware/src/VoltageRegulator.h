#ifndef __CLUCK3SESAME_SRC_VOLTAGEREGULATOR_H
#define __CLUCK3SESAME_SRC_VOLTAGEREGULATOR_H
#include "Event.h"

#define VOLTAGE_REGULATOR_ENABLED ((EventType) 0x28)
struct VoltageRegulatorEnabled { EMPTY_EVENT_ARGS };

#define VOLTAGE_REGULATOR_DISABLED ((EventType) 0x29)
struct VoltageRegulatorDisabled { EMPTY_EVENT_ARGS };

extern void voltageRegulatorInitialise(void);
extern void voltageRegulatorEnable(void);
extern void voltageRegulatorDisable(void);

#endif
