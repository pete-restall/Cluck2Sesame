#include <xc.h>
#include <stdint.h>

#include "Event.h"
#include "NearScheduler.h"

#define NCO_32768HZ_10MS_INCREMENT 3200
#define NCOCLK_SOURCE_SOSC (0b0101 << _NCO1CLK_N1CKS_POSITION)

void nearSchedulerInitialise(void)
{
	NCO1CON = 0;
	NCO1CLK = NCOCLK_SOURCE_SOSC;
	NCO1INCU = 0;
	NCO1INCH = (uint8_t) ((NCO_32768HZ_10MS_INCREMENT >> 8) & 0xff);
	NCO1INCL = (uint8_t) ((NCO_32768HZ_10MS_INCREMENT >> 0) & 0xff);
	PIR7bits.NCO1IF = 0;
	PIE7bits.NCO1IE = 1;
}

void nearSchedulerAdd(const struct NearSchedule *const schedule)
{
	if (!NCO1CONbits.N1EN)
	{
		NCO1ACCU = 0;
		NCO1ACCH = 0;
		NCO1ACCL = 0;
		NCO1CONbits.N1EN = 1;
	}
}
