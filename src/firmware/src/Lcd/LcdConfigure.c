#include <xc.h>
#include <stdint.h>

#include "../NearScheduler.h"

#include "Lcd.h"

#define LCD_NYBBLE_CMD 0b00000000
#define LCD_NYBBLE_DATA 0b10000000

static void lcdConfigureStateMachine(void *const state);
static void lcdWriteNybble(uint8_t nybble);
static void lcdWriteCommand(uint8_t byte);

void lcdConfigure(void)
{
	static const struct NearSchedule waitForLcdToStabilise =
	{
		.ticks = MS_TO_TICKS(48),
		.handler = lcdConfigureStateMachine
	};

	nearSchedulerAdd(&waitForLcdToStabilise);
	PWM5CONbits.PWM5EN = 1;
}

static void lcdConfigureStateMachine(void *const state)
{
	static const struct LcdEnabled emptyEventArgs = { };

	struct NearSchedule waitForLcdCommand =
	{
		.ticks = MS_TO_TICKS(8),
		.handler = lcdConfigureStateMachine,
		.state = (void *) (((int) state) + 1)
	};

	switch ((int) state)
	{
		case 0:
		case 1:
		case 2:
			lcdWriteNybble(LCD_NYBBLE_CMD | 0b00000011);
			break;

		case 3:
			lcdWriteNybble(LCD_NYBBLE_CMD | 0b00000010);
			break;

		case 4:
			lcdWriteCommand(
				LCD_CMD_FUNCTION |
				LCD_CMD_FUNCTION_TWOLINES |
				LCD_CMD_FUNCTION_FONT5X8);
			break;

		case 5:
			lcdWriteCommand(LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_OFF);
			break;

		case 6:
			lcdWriteCommand(LCD_CMD_DISPLAYCLEAR);
			break;

		case 7:
			lcdWriteCommand(
				LCD_CMD_ENTRYMODE |
				LCD_CMD_ENTRYMODE_INCREMENT |
				LCD_CMD_ENTRYMODE_NOSHIFT);
			break;

		case 8:
			lcdWriteCommand(LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_ON);
			break;

		default:
			eventPublish(LCD_ENABLED, &emptyEventArgs);
			waitForLcdCommand.handler = (NearScheduleHandler) 0;
	}

	nearSchedulerAdd(&waitForLcdCommand);
}

static void lcdWriteNybble(uint8_t nybble)
{
	LATAbits.LATA4 = 1;
	LATAbits.LATA3 = (nybble & 0b10000000) ? 1 : 0;
	LATCbits.LATC4 = (nybble & 0b00001000) ? 1 : 0;
	LATAbits.LATA6 = (nybble & 0b00000100) ? 1 : 0;
	LATAbits.LATA7 = (nybble & 0b00000010) ? 1 : 0;
	LATAbits.LATA5 = (nybble & 0b00000001) ? 1 : 0;
	LATAbits.LATA4 = 0;
}

static void lcdWriteCommand(uint8_t byte)
{
	lcdWriteNybble(LCD_NYBBLE_CMD | ((byte >> 4) & 0b00001111));
	lcdWriteNybble(LCD_NYBBLE_CMD | ((byte >> 0) & 0b00001111));
}

// TODO: IN lcdWriteCommand() WE PROBABLY WANT SOMETHING LIKE THIS...?
#if 0
/*
#define LCD_CMD_MASK_FOR_SHORTDELAY 0b11111100

if (command & LCD_CMD_MASK_FOR_SHORT_DELAY)
	command can use TMR2 quick wait (128us)
else
	command needs to use nearSchedulerAdd(MS_TO_TICKS(8))
*/
#endif
