#include <xc.h>
#include <stdint.h>

#include "Motor.h"

#define CCP1CON_MODE_MASK (0b1111 << _CCP1CON_MODE_POSITION)
#define CCP1CON_COMPARE_AND_SET_MODE (0b1000 << _CCP1CON_MODE_POSITION)

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

	CCP1CON &= ~CCP1CON_MODE_MASK;
	CCPR1H = (uint8_t) ((count >> 8) & 0xff);
	CCPR1L = (uint8_t) ((count >> 0) & 0xff);
	TMR1H = 0;
	TMR1L = 0;
	PIR4bits.TMR1IF = 0;
	CCP1CON |= CCP1CON_COMPARE_AND_SET_MODE;
}

void motorOff(void)
{
}