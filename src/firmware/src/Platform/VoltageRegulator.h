#ifndef __CLUCK3SESAME_SRC_PLATFORM_VOLTAGEREGULATOR_H
#define __CLUCK3SESAME_SRC_PLATFORM_VOLTAGEREGULATOR_H
#include "Event.h"

#define VOLTAGE_REGULATOR_ENABLED ((EventType) 0x18)
struct VoltageRegulatorEnabled { EMPTY_EVENT_ARGS };

#define VOLTAGE_REGULATOR_DISABLED ((EventType) 0x19)
struct VoltageRegulatorDisabled { EMPTY_EVENT_ARGS };

extern void voltageRegulatorInitialise(void);
extern void voltageRegulatorEnable(void);
extern uint8_t voltageRegulatorIsEnabled(void);
extern void voltageRegulatorDisable(void);

#endif
