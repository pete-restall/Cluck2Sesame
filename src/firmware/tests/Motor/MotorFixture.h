#ifndef __CLUCK3SESAME_TESTS_MOTOR_MOTORFIXTURE_H
#define __CLUCK3SESAME_TESTS_MOTOR_MOTORFIXTURE_H
#include <stdint.h>
#include "NearScheduler.h"
#include "VoltageRegulator.h"
#include "Motor.h"

#define STEERING_MASK ( \
	_CWG1STR_STRA_MASK | \
	_CWG1STR_STRB_MASK | \
	_CWG1STR_STRC_MASK | \
	_CWG1STR_STRD_MASK)

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
extern uint8_t onMotorStartedCalls;
extern struct MotorStarted onMotorStartedArgs;
extern uint8_t onMotorStoppedCalls;
extern struct MotorStopped onMotorStoppedArgs;
extern uint16_t nearSchedulerAddCalls;
extern const struct NearSchedule *nearSchedulerAddArgs[8];

extern void motorFixtureSetUp(void);
extern void motorFixtureTearDown(void);
extern void publishVoltageRegulatorEnabled(void);
extern void dispatchAllEvents(void);
extern void publishVoltageRegulatorDisabled(void);
extern void stubVoltageRegulatorIsEnabled(uint8_t value);
extern void mockOnVoltageRegulatorDisabled(void);
extern void stubVoltageRegulatorDisableToPublishEvent(void);

extern void ensureMotorFullyEnabled(void);
extern int16_t anyClockwiseCount(void);
extern int16_t anyAntiClockwiseCount(void);
extern int16_t anyEncoderCount(void);
extern void publishWokenFromSleep(void);

#endif
