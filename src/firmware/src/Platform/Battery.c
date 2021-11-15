#include <xc.h>
#include <stdint.h>

#include "Event.h"
#include "Battery.h"

void batteryInitialise(void)
{
	ANSELB &= ~(_ANSELB_ANSB3_MASK | _ANSELB_ANSB4_MASK | _ANSELB_ANSB5_MASK);
	LATBbits.LATB3 = 0;
	TRISBbits.TRISB3 = 0;
	TRISBbits.TRISB4 = 1;
	TRISBbits.TRISB5 = 1;
	INLVLBbits.INLVLB4 = 0;
	INLVLBbits.INLVLB5 = 0;
}

// TODO: INTERRUPT ON CHANGE

/*
TODO:
CHARGE BETWEEN 5degC AND 30degC.  CHARGE TO 3.9V AND DISCHARGE TO 3.0V
RB5 = /CHARGER_GOOD
RB4 = /CHARGING
RB3 = CHARGER_EN

IF /CHARGER_GOOD && TEMP >= 5 && TEMP <= 30 && VBATT <= 3.8 && LAST_CHARGE_OFF_TIME >= 1MIN (IE. AT CLOCK OVERFLOW) THEN CHARGER_EN = 1
ELSE CHARGER_EN = 0, LAST_CHARGE_OFF_TIME = CURRENT TIMESTAMP
*/