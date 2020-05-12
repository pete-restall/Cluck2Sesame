/* NOTE: The ceedling tests do not cover a lot of this file as it is complex due to the asynchronous nature of the motor
   control.  If you make changes then there is no substitute for bench testing the motor as it is quicker and simpler
   and guaranteed to reflect the real world. */

#include <xc.h>
#include <stdint.h>

#include "../Event.h"
#include "../NearScheduler.h"

#include "Motor.h"

#define PWM_INCREMENT_TICK_COUNT 10
#define ENCODER_STATIC_COUNT_LIMIT 10

#define CCP1CON_MODE_MASK (0b1111 << _CCP1CON_MODE_POSITION)
#define CCP1CON_COMPARE_AND_SET_MODE (0b1000 << _CCP1CON_MODE_POSITION)

#define MOTOR_RUNNING_STATE_STOPPED 0
#define MOTOR_RUNNING_STATE_STARTING 1
#define MOTOR_RUNNING_STATE_STARTED 2

static void incrementPwmDutyCycle(void *state);
static void motorRunningStateMonitor(void *state);

static struct MotorStopped motorStoppedEventArgs;
static uint8_t motorRunningState;
static struct MotorStarted motorStartedEventArgs;

static const struct NearSchedule pwmDutyCycleIncrementingSchedule =
{
	.ticks = PWM_INCREMENT_TICK_COUNT,
	.handler = &incrementPwmDutyCycle
};

static const struct NearSchedule motorRunningStateMonitorSchedule =
{
	.ticks = 32 * PWM_INCREMENT_TICK_COUNT / ENCODER_STATIC_COUNT_LIMIT,
	.handler = &motorRunningStateMonitor
};

void motorOn(int16_t count)
{
	if (count == 0 || (CWG1STR & 0x0f) || motorRunningState != MOTOR_RUNNING_STATE_STOPPED)
		return;

	motorRunningState = MOTOR_RUNNING_STATE_STARTING;
	motorStartedEventArgs.count = count;
	eventPublish(MOTOR_STARTED, &motorStartedEventArgs);

	uint8_t steeringMask = _CWG1STR_STRA_MASK;
	if (count < 0)
	{
		steeringMask = _CWG1STR_STRB_MASK;
		count = -count;
		if (count == -32768)
			count = 0x7fff;
	}

	CCP1CON &= ~CCP1CON_MODE_MASK;
	PWM4DCH = 0;
	PWM4DCL = 0;
	CCPR1H = (uint8_t) ((count >> 8) & 0xff);
	CCPR1L = (uint8_t) ((count >> 0) & 0xff);
	TMR1H = 0;
	TMR1L = 0;
	CCP1CON |= CCP1CON_COMPARE_AND_SET_MODE;


	/* *********************************************************************
	   ALERT: The interrupt flags need to be cleared *AFTER* the CCP mode
	   change, see section 28.1.3 of the datasheet for the single sentence
	   that alludes to this. */

	PIR4bits.TMR1IF = 0;
	PIR2bits.C1IF = 0;
	PIR6bits.CCP1IF = 0;
	PIR5bits.CLC2IF = 0;
	CWG1AS0bits.SHUTDOWN = 0;
	PIR7bits.CWG1IF = 0;

	/* ******************************************************************* */

	CWG1STR |= steeringMask;

	nearSchedulerAddOrUpdate(&pwmDutyCycleIncrementingSchedule);
	nearSchedulerAddOrUpdate(&motorRunningStateMonitorSchedule);
}

static void incrementPwmDutyCycle(void *state)
{
	if (!(CWG1STR & CWG1STR_STEERING_MASK))
		return;

	if (PWM4DCH != (256 - 8) >> 2)
		nearSchedulerAddOrUpdate(&pwmDutyCycleIncrementingSchedule);

	PWM4DCH += 8 >> 2;
}

void motorOff(void)
{
	if (CWG1STR & CWG1STR_STEERING_MASK)
	{
		CWG1STR &= ~CWG1STR_STEERING_MASK;
		motorStoppedEventArgs.requestedCount = motorStartedEventArgs.count;
		motorStoppedEventArgs.fault.all = 0;
		motorStoppedEventArgs.fault.currentLimited = PIR2bits.C1IF != 0 ? 1 : 0;
		motorStoppedEventArgs.fault.encoderOverflow = PIR4bits.TMR1IF != 0 ? 1 : 0;
	}

	PIR2bits.C1IF = 0;
	PIR4bits.TMR1IF = 0;
	PIR7bits.CWG1IF = 0;
}

static void motorRunningStateMonitor(void *state)
{
	static uint16_t previousEncoderValue;
	static uint8_t encoderStaticCount;

	uint16_t encoderValue = (uint16_t) TMR1L;
	encoderValue |= ((uint16_t) TMR1H) << 8;

	if (motorRunningState == MOTOR_RUNNING_STATE_STARTED && encoderValue == previousEncoderValue)
	{
		if (++encoderStaticCount == ENCODER_STATIC_COUNT_LIMIT)
		{
			motorStoppedEventArgs.actualCount = encoderValue;
			if (motorStoppedEventArgs.requestedCount < 0)
			{
				motorStoppedEventArgs.actualCount = -motorStoppedEventArgs.actualCount;
				if (motorStoppedEventArgs.actualCount > motorStoppedEventArgs.requestedCount && motorStoppedEventArgs.fault.all == 0)
					motorStoppedEventArgs.fault.encoderTimeout = 1;
			}
			else
			{
				if (motorStoppedEventArgs.actualCount < motorStoppedEventArgs.requestedCount && motorStoppedEventArgs.fault.all == 0)
					motorStoppedEventArgs.fault.encoderTimeout = 1;
			}

			eventPublish(MOTOR_STOPPED, &motorStoppedEventArgs);
			motorRunningState = MOTOR_RUNNING_STATE_STOPPED;
			return;
		}
	}
	else
	{
		if (motorRunningState == MOTOR_RUNNING_STATE_STARTING)
			motorRunningState = MOTOR_RUNNING_STATE_STARTED;

		encoderStaticCount = 0;
	}

	previousEncoderValue = encoderValue;
	nearSchedulerAddOrUpdate(&motorRunningStateMonitorSchedule);
}
