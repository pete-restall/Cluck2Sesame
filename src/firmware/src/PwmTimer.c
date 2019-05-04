#include <xc.h>
#include <stdint.h>

#include "PwmTimer.h"

#define T2CON_PRESCALER_2 (0b001 << _T2CON_CKPS_POSITION)
#define T2CON_POSTSCALER_8 (0b0111 << _T2CON_OUTPS_POSITION)

#define T2CLKCON_FOSC_OVER_4 0b00000001

#define T2HLT_NO_PRESCALER_SYNC 0
#define T2HLT_RISING_EDGE 0
#define T2HLT_ON_SYNC _T2HLT_CKSYNC_MASK
#define T2HLT_MODE_FREE 0
#define T2HLT_MODE_SOFTGATE 0

static uint8_t enableCount;

void pwmTimerInitialise(void)
{
	T2CON = T2CON_PRESCALER_2 | T2CON_POSTSCALER_8;
	T2CLKCON = T2CLKCON_FOSC_OVER_4;
	T2HLT =
		T2HLT_NO_PRESCALER_SYNC |
		T2HLT_RISING_EDGE |
		T2HLT_ON_SYNC |
		T2HLT_MODE_FREE |
		T2HLT_MODE_SOFTGATE;

	PR2 = 63;
	PIR4bits.TMR2IF = 0;
	PIE4bits.TMR2IE = 1;
	enableCount = 0;
}

void pwmTimerEnable(void)
{
	if (enableCount++ > 0)
		return;

	PIR4bits.TMR2IF = 0;
	T2CONbits.ON = 1;
}

void pwmTimerDisable(void)
{
	if (enableCount <= 1)
	{
		T2CONbits.ON = 0;
		PIR4bits.TMR2IF = 0;
		enableCount = 0;
	}
	else
		--enableCount;
}
