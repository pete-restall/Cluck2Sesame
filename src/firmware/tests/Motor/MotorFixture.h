#ifndef __CLUCK3SESAME_TESTS_MOTOR_MOTORFIXTURE_H
#define __CLUCK3SESAME_TESTS_MOTOR_MOTORFIXTURE_H
#include <stdint.h>
#include "VoltageRegulator.h"

extern uint8_t voltageRegulatorIsEnabledValue;
extern uint8_t voltageRegulatorEnableCalls;
extern uint8_t voltageRegulatorEnableSequence;
extern uint8_t voltageRegulatorDisableCalls;
extern uint8_t voltageRegulatorDisableSequence;
extern uint8_t onVoltageRegulatorDisabledCalls;
extern uint8_t onVoltageRegulatorDisabledSequence;
extern uint8_t onMotorEnabledCalls;
extern uint8_t onMotorEnabledSequence;
extern uint8_t onMotorDisabledCalls;
extern uint8_t onMotorDisabledSequence;

extern void motorFixtureSetUp(void);
extern void motorFixtureTearDown(void);
extern void publishVoltageRegulatorEnabled(void);
extern void dispatchAllEvents(void);
extern void publishVoltageRegulatorDisabled(void);
extern void stubVoltageRegulatorIsEnabled(uint8_t value);
extern void mockOnVoltageRegulatorDisabled(void);
extern void stubVoltageRegulatorDisableToPublishEvent(void);

#endif
