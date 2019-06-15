#include <xc.h>
#include <stdint.h>

#include "Adc.h"

#define ADCON1_RIGHT_JUSTIFIED _ADCON1_ADFM_MASK
#define ADCON1_TAD_1US (0b010 << _ADCON1_ADCS_POSITION)

void adcInitialise(void)
{
}

void adcSample(struct AdcSample *sample)
{
	if (!sample)
		return;

	PMD2bits.ADCMD = 0;
	ADCON0 = sample->channel;
	ADCON1 = ADCON1_RIGHT_JUSTIFIED | ADCON1_TAD_1US;
	ADACT = 0;
	for (uint8_t i = 0; i < 10; i++)
		asm("nop");

	sample->result = 0;
	ADCON0bits.GOnDONE = 1;
	for (uint8_t i = 0; i < sample->count; i++)
	{
		while (ADCON0bits.GOnDONE)
			;;

		ADCON0bits.GOnDONE = 1;
		sample->result += ADRES;
	}

	PMD2bits.ADCMD = 1;
}
