#ifndef __CLUCK3SESAME_SRC_PLATFORM_LCD_H
#define __CLUCK3SESAME_SRC_PLATFORM_LCD_H
#include <stdint.h>
#include "Event.h"

#define LCD_ENABLED ((EventType) 0x30)
struct LcdEnabled { EMPTY_EVENT_ARGS };

#define LCD_CMD_FUNCTION 0b00100000
#define LCD_CMD_FUNCTION_TWOLINES 0b00001000
#define LCD_CMD_FUNCTION_FONT5X8 0b00000000

#define LCD_CMD_DISPLAY 0b00001000
#define LCD_CMD_DISPLAY_OFF 0b00000000
#define LCD_CMD_DISPLAY_ON 0b00000100

#define LCD_CMD_DISPLAYCLEAR 0b00000001

#define LCD_CMD_ENTRYMODE 0b00000100
#define LCD_CMD_ENTRYMODE_INCREMENT 0b00000010
#define LCD_CMD_ENTRYMODE_NOSHIFT 0b00000000

#define LCD_CMD_SETDDRAMADDRESS 0b10000000
#define LCD_CMD_SETDDRAMADDRESS_LINE1 LCD_CMD_SETDDRAMADDRESS
#define LCD_CMD_SETDDRAMADDRESS_LINE2 (LCD_CMD_SETDDRAMADDRESS | 0x40)

#define LCD_CMD_DONE 0

typedef void (*LcdTransactionCallback)(void *state);

struct LcdPutsTransaction
{
	const uint8_t *buffer;
	LcdTransactionCallback callback;
	void *state;
};

struct LcdSetAddressTransaction
{
	uint8_t address;
	LcdTransactionCallback callback;
	void *state;
};

extern void lcdInitialise(void);
extern void lcdEnable(void);
extern void lcdDisable(void);
extern void lcdPuts(const struct LcdPutsTransaction *const transaction);
extern void lcdSetDdramAddress(const struct LcdSetAddressTransaction *const transaction);

#endif
