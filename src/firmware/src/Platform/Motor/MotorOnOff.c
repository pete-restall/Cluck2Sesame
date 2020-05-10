#include <xc.h>
#include <stdint.h>

#include "../Event.h"
#include "../NearScheduler.h"

#include "Motor.h"

#define CCP1CON_MODE_MASK (0b1111 << _CCP1CON_MODE_POSITION)
#define CCP1CON_COMPARE_AND_SET_MODE (0b1000 << _CCP1CON_MODE_POSITION)

static void incrementPwmDutyCycle(void *state);

static struct MotorStarted motorStartedEventArgs;

static const struct NearSchedule pwmDutyCycleIncrementingSchedule =
{
	.ticks = 10, // TODO: TESTS WILL NEED UPDATING FOR THIS...(WAS 1, NOW 10)
	.handler = &incrementPwmDutyCycle
};

void motorOn(int16_t count)
{
	if (count == 0 || (CWG1STR & 0x0f))
		return;

	motorStartedEventArgs.count = count;
	eventPublish(MOTOR_STARTED, &motorStartedEventArgs);

	uint8_t steeringMask = _CWG1STR_STRA_MASK; // TODO: A AND B HAVE BEEN SWITCHED AROUND AS THEY WERE WRONG...FIX THE TESTS...
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
	PIR2bits.C1IF = 0; // TODO: SHOULD REALLY CLEAR THIS HERE - COMPARATOR CAN BE TWITCHY...
	PIR6bits.CCP1IF = 0; // TODO: NEEDS ADDING TO THE TESTS
	PIR5bits.CLC2IF = 0; // TODO: NEEDS ADDING TO THE TESTS
	CWG1AS0bits.SHUTDOWN = 0; // TODO: NEEDS ADDING TO THE TESTS
	PIR7bits.CWG1IF = 0; // TODO: NEEDS ADDING TO THE TESTS

	/* ******************************************************************* */

	CWG1STR |= steeringMask;

	nearSchedulerAdd(&pwmDutyCycleIncrementingSchedule);
}

static void incrementPwmDutyCycle(void *state)
{
	if (!(CWG1STR & CWG1STR_STEERING_MASK))
		return;

	if (PWM4DCH != (256 - 8) >> 2)
		nearSchedulerAdd(&pwmDutyCycleIncrementingSchedule);

	PWM4DCH += 8 >> 2;
}

void motorOff(void)
{
	if (CWG1STR & CWG1STR_STEERING_MASK)
	{
		uint8_t antiClockwise = CWG1STRbits.STRB; // TODO: A AND B HAVE BEEN SWITCHED AROUND AS THEY WERE WRONG...FIX THE TESTS...
		CWG1STR &= ~CWG1STR_STEERING_MASK;

		static struct MotorStopped eventArgs;
		eventArgs.fault.currentLimited = PIR2bits.C1IF != 0 ? 1 : 0;
		eventArgs.fault.encoderOverflow = PIR4bits.TMR1IF != 0 ? 1 : 0;
		// TODO: eventArgs.fault.encoderTimeout = ???
		eventArgs.requestedCount = motorStartedEventArgs.count;
		eventArgs.actualCount = (int16_t) ((uint16_t) TMR1L);
		eventArgs.actualCount |= ((int16_t) TMR1H) << 8;
		if (antiClockwise)
			eventArgs.actualCount = -eventArgs.actualCount;

		eventPublish(MOTOR_STOPPED, &eventArgs);
	}

	PIR2bits.C1IF = 0;
	PIR4bits.TMR1IF = 0;
	PIR7bits.CWG1IF = 0;
}
