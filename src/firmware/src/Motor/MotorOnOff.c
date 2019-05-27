#include <xc.h>
#include <stdint.h>

#include "Motor.h"

void motorOn(int16_t count)
{
	if (count == 0)
		return;

	if (count < 0)
	{
		count = -count;
		if (count == -32768)
			count = 0x7fff;
	}

	CCPR1H = (uint8_t) ((count >> 8) & 0xff);
	CCPR1L = (uint8_t) ((count >> 0) & 0xff);
	TMR1H = 0;
	TMR1L = 0;
	PIR4bits.TMR1IF = 0;

	// TODO: 'CCP1CON &= 0xf0; CCPR1 = xxx; CCP1CON |= 0x08;' THAT'S THE
	// SEQUENCE REQUIRED TO RESET THE CCP1CON OUTPUT, NECESSARY BECAUSE THE
	// 'SHUTDOWN' BIT DOESN'T GET CLEARED UNTIL THE 'TMR1 == CCPR1' CONDITION
	// IS CLEARED, AND 'TMR1' WILL NOT INCREMENT UNTIL A RISING EDGE, WHICH
	// WON'T HAPPEN BECAUSE THE MOTOR IS NOT TURNING.  EVEN MANUALLY SETTING
	// TMR1 DOES NOT CLEAR THE CONDITION.  THIS NEEDS TO BE TESTED FOR.
}

void motorOff(void)
{
}
