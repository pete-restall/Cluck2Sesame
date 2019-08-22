#include <xc.h>

#include "Main.h"

#ifndef TEST

#include <stdint.h> // TODO: TEMPORARY !!!
#include <stdlib.h> // TODO: TEMPORARY !!!
#include "Event.h" // TODO: TEMPORARY !!!
#include "NearScheduler.h" // TODO: TEMPORARY !!!
#include "Clock.h" // TODO: TEMPORARY !!!
#include "Lcd.h" // TODO: TEMPORARY !!!
#include "Adc.h" // TODO: TEMPORARY !!!
#include "Motor.h" // TODO: TEMPORARY !!!

static uint16_t initialFvrResult = 0;
static uint16_t fvrResult = 0;
static uint16_t initialTemperatureResult = 0;
static uint16_t temperatureResult = 0;

static char screen[2 * 17] =
{
	'F', 'V', 'R', '=', '0', '0', '0', '0', ' ', 'T', '=', '0', '0', '0', '0', ' ', '\0',
	'Y', 'Y', 'M', 'M', '-', 'D', 'D', ' ', 'H', 'H', ':', 'M', 'M', ':', 'S', 'S', '\0'
};

static void updateScreen(void *unused);

static void displayTwoHexDigits(uint8_t at, uint8_t value)
{
	uint8_t upper = (value >> 4) & 0x0f;
	uint8_t lower = (value >> 0) & 0x0f;

	screen[at] = ((upper > 9) ? ('A' - 10) : '0') + upper;
	screen[at + 1] = ((lower > 9) ? ('A' - 10) : '0') + lower;
}

static void displayTwoDigits(uint8_t at, uint8_t value)
{
	div_t digits = div(value, 10);
	screen[at] = '0' + digits.quot;
	screen[at + 1] = '0' + digits.rem;
}

static void secondLine(void *state)
{
	static const struct LcdPutsTransaction t =
	{
		.buffer = &screen[17],
		.callback = &updateScreen
	};

	lcdPuts(&t);
}

static void gotoSecondLine(void *state)
{
	static const struct LcdSetAddressTransaction t =
	{
		.address = 0x40,
		.callback = &secondLine
	};

	lcdSetDdramAddress(&t);
}

static void firstLine(void *state)
{
	static const struct LcdPutsTransaction t =
	{
		.buffer = &screen[0],
		.callback = &gotoSecondLine
	};

	lcdPuts(&t);
}

static void updateScreen(void *unused)
{
	static const struct LcdSetAddressTransaction t =
	{
		.address = 0x00,
		.callback = &firstLine
	};

	lcdSetDdramAddress(&t);

	displayTwoHexDigits(0 + 4, (uint8_t) ((fvrResult >> 8) & 0xff));
	displayTwoHexDigits(0 + 6, (uint8_t) ((fvrResult >> 0) & 0xff));
	displayTwoHexDigits(0 + 11, (uint8_t) ((temperatureResult >> 8) & 0xff));
	displayTwoHexDigits(0 + 13, (uint8_t) ((temperatureResult >> 0) & 0xff));

	struct DateAndTimeGet now;
	clockGetNowGmt(&now);
	displayTwoDigits(17 + 0, now.date.year);
	displayTwoDigits(17 + 2, now.date.month);
	displayTwoDigits(17 + 5, now.date.day);
	displayTwoDigits(17 + 8, now.time.hour);
	displayTwoDigits(17 + 11, now.time.minute);
	displayTwoDigits(17 + 14, now.time.second);
	screen[15] = screen[15] + 1;
}

static void onLcdEnabled(const struct Event *event)
{
	updateScreen((void *) 0);
}

void main(void)
{
	initialise();

static const struct DateAndTimeSet d =
{
	.date =
	{
		.year = 19,
		.month = 5,
		.day = 24
	},
	.time =
	{
		.hour = 14,
		.minute = 25,
		.second = 0
	}
};

clockSetNowGmt(&d);
//goto nonHack;

LATBbits.LATB7 = 0;
TRISBbits.TRISB7 = 0;
ANSELBbits.ANSB7 = 0;
//RB7PPS = 0x19;
LATBbits.LATB6 = 0;
TRISBbits.TRISB6 = 0;
ANSELBbits.ANSB6 = 0;
PMD0bits.CLKRMD = 0;
RB6PPS = 0x1b;
CLKRCLK = 0b00000101;
CLKRCON = 0b10010000;
static const struct EventSubscription lcdEnabled = { .type = LCD_ENABLED, .handler = &onLcdEnabled };
eventSubscribe(&lcdEnabled);
ANSELAbits.ANSA0 = 0;
ANSELAbits.ANSA1 = 0;
//RB7PPS = 0x0c;

PMD0bits.FVRMD = 0;
FVRCON = 0b10110010;

uint8_t initialReadingsTaken = 0;
uint8_t adcState = 0;
uint8_t channel = ADC_CHANNEL_ADCFVR;
struct AdcSample sample = { .count = 8 };
while (1)
{
	if (initialReadingsTaken == 10)
	{
		lcdEnable();
		motorEnable();
		initialReadingsTaken = 0xff;
	}
	else if (initialReadingsTaken != 0xff)
		eventPublish(0xff, (void *) 0);

	if (adcState == 0)
	{
		if (channel == ADC_CHANNEL_ADCFVR)
			channel = ADC_CHANNEL_TEMPERATURE;
		else
			channel = ADC_CHANNEL_ADCFVR;

		sample.channel = channel;
		adcSample(&sample);
		adcState = 1;
	}
	else if (adcState == 1)
	{
		if (channel == ADC_CHANNEL_ADCFVR)
		{
			if (initialReadingsTaken == 0xff)
				fvrResult = sample.result / 8;
			else
				initialFvrResult = sample.result / 8;
		}
		else
		{
			if (initialReadingsTaken == 0xff)
				temperatureResult = sample.result / 8;
			else
				initialTemperatureResult = sample.result / 8;
		}

		if (initialReadingsTaken != 0xff)
			initialReadingsTaken++;

		adcState = 0;
	}

	if (initialReadingsTaken == 0xff)
	{
		if (PORTAbits.RA0 == 0)
			motorOn(11 * 40);
		else if (PORTAbits.RA1 == 0)
			motorOn(-11 * 40);
		else
			motorOff();

		LATBbits.LATB7 = CWG1AS0bits.SHUTDOWN != 0 ? 1 : 0;
	}

	poll();
}


nonHack:
	while (1)
		poll();
}

#endif
