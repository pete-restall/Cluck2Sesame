#include <xc.h>

#include "Main.h"

#ifndef TEST

#include <stdint.h> // TODO: TEMPORARY !!!
#include <stdlib.h> // TODO: TEMPORARY !!!
#include "Event.h" // TODO: TEMPORARY !!!
#include "NearScheduler.h" // TODO: TEMPORARY !!!
#include "Clock.h" // TODO: TEMPORARY !!!
#include "Lcd.h" // TODO: TEMPORARY !!!
#include "Motor.h" // TODO: TEMPORARY !!!

static uint8_t screen[2 * 17] =
{
	'H', 'e', 'l', 'l', 'o', ',', ' ', 'P', 'e', 't', 'e', ' ', '!', ' ', ' ', ' ', '\0',
	'Y', 'Y', 'M', 'M', '-', 'D', 'D', ' ', 'H', 'H', ':', 'M', 'M', ':', 'S', 'S', '\0'
};

static void updateScreen(void *const unused);

static void displayTwoDigits(uint8_t at, uint8_t value)
{
	div_t digits = div(value, 10);
	screen[at] = '0' + digits.quot;
	screen[at + 1] = '0' + digits.rem;
}

static void secondLine(void *const state)
{
	static const struct LcdPutsTransaction t =
	{
		.buffer = (const uint8_t *) &screen[17],
		.callback = &updateScreen
	};

	lcdPuts(&t);
}

static void gotoSecondLine(void *const state)
{
	static const struct LcdSetAddressTransaction t =
	{
		.address = 0x40,
		.callback = secondLine
	};

	lcdSetDdramAddress(&t);
}

static void firstLine(void *const state)
{
	static const struct LcdPutsTransaction t =
	{
		.buffer = (const uint8_t *) &screen[0],
		.callback = &gotoSecondLine
	};

	lcdPuts(&t);
}

static void updateScreen(void *const unused)
{
	static const struct LcdSetAddressTransaction t =
	{
		.address = 0x00,
		.callback = firstLine
	};

	lcdSetDdramAddress(&t);

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

static void onLcdEnabled(const struct Event *const event)
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
lcdEnable();
ANSELAbits.ANSA0 = 0;
ANSELAbits.ANSA1 = 0;
//RB7PPS = 0x0c;
while (1)
{
	if (PORTAbits.RA0 == 0)
		motorOn(11 * 40);
	else if (PORTAbits.RA1 == 0)
		motorOn(-11 * 40);
	else
		motorOff();

	LATBbits.LATB7 = CWG1AS0bits.SHUTDOWN != 0 ? 1 : 0;
	poll();
}


nonHack:
	while (1)
		poll();
}

#endif
