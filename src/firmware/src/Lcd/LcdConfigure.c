#include <xc.h>
#include <stdint.h>

#include "../NearScheduler.h"

#include "Lcd.h"

#define LCD_NYBBLE_INSTRUCTION 0b00000000
#define LCD_NYBBLE_DATA 0b10000000

static void lcdConfigureAsNybbleMode(void *const state);

void lcdConfigure(void)
{
	static const struct NearSchedule waitForLcdToStabilise =
	{
		.ticks = MS_TO_TICKS(48),
		.handler = lcdConfigureAsNybbleMode
	};

	nearSchedulerAdd(&waitForLcdToStabilise);
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

static void lcdConfigureAsNybbleMode(void *const state)
{
	lcdWriteNybble(LCD_NYBBLE_INSTRUCTION | 0b00000011);
//	// wait 8ms
//	rs=0, db7-4=0011
//	// wait 8ms
//	rs=0, db7-4=0011
//	// wait 8ms
//	rs=0, db7-4=0010
//	// wait 8ms
}

#if 0
/*
	static const uint8_t configuration[] =
	{
		LCD_CMD_FUNCTION | LCD_CMD_FUNCTION_TWOLINES | LCD_CMD_FUNCTION_FONT5X8,
		LCD_CMD_DISPLAYOFF,
		LCD_CMD_DISPLAYCLEAR,
		LCD_CMD_ENTRYMODE | LCD_CMD_ENTRYMODE_INCREMENT | LCD_ENTRYMODE_NOSHIFT,
		LCD_CMD_DONE
	};

	lcdSendCommands(configuration);
*/
}

/*
#define LCD_CMD_MASK_FOR_SHORTDELAY 0b11111100

if (command & LCD_CMD_MASK_FOR_SHORT_DELAY)
	command can use TMR2 quick wait (128us)
else
	command needs to use nearSchedulerAdd(MS_TO_TICKS(8))
*/
#endif
