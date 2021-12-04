#ifndef __CLUCK3SESAME_SRC_PLATFORM_ADC_H
#define __CLUCK3SESAME_SRC_PLATFORM_ADC_H
#include <stdint.h>

#define ADC_CHANNEL_ADCFVR 0b11111001
#define ADC_CHANNEL_CMPFVR 0b11111101
#define ADC_CHANNEL_TEMPERATURE 0b11110001
#define ADC_CHANNEL_DAC 0b11110101
#define ADC_CHANNEL_VSS 0b11101101
#define ADC_CHANNEL_RB1 0b00100101

struct AdcSample
{
	uint8_t count;
	uint8_t channel;
	uint16_t result;
	union
	{
		uint8_t all;
		uint8_t any;
		struct
		{
			unsigned int acquisitionTimeMultiple : 4;
			unsigned int vrefIsFvr : 1;
		};
	} flags;
};

extern void adcInitialise(void);
extern void adcSample(struct AdcSample *sample);

#endif
