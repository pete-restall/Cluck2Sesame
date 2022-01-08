#ifndef __CLUCK2SESAME_SRC_PLATFORM_LCD_H
#define __CLUCK2SESAME_SRC_PLATFORM_LCD_H
#include <stdint.h>
#include "Event.h"

#define LCD_ENABLED ((EventType) 0x20)
struct LcdEnabled { EMPTY_EVENT_ARGS };

#define LCD_DISABLED ((EventType) 0x21)
struct LcdDisabled { EMPTY_EVENT_ARGS };

#define LCD_ADDRESS_LINE1 0x00
#define LCD_ADDRESS_LINE2 0x40

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
#define LCD_CMD_SETDDRAMADDRESS_LINE1 (LCD_CMD_SETDDRAMADDRESS | LCD_ADDRESS_LINE1)
#define LCD_CMD_SETDDRAMADDRESS_LINE2 (LCD_CMD_SETDDRAMADDRESS | LCD_ADDRESS_LINE2)

#define LCD_CMD_DONE 0

typedef void (*LcdTransactionCallback)(void *state);

struct LcdPutsTransaction
{
	const char *buffer;
	LcdTransactionCallback callback;
	void *state;
};

struct LcdSetAddressTransaction
{
	uint8_t address;
	LcdTransactionCallback callback;
	void *state;
};

struct LcdSetCursorTransaction
{
	uint8_t on;
	LcdTransactionCallback callback;
	void *state;
};

extern void lcdInitialise(void);
extern void lcdEnable(void);
extern void lcdDisable(void);
extern void __reentrant lcdPuts(const struct LcdPutsTransaction *transaction);
extern void lcdSetDdramAddress(const struct LcdSetAddressTransaction *transaction);
extern void lcdSetCursor(const struct LcdSetCursorTransaction *transaction);

#endif
