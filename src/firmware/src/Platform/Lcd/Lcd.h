#ifndef __CLUCK2SESAME_SRC_PLATFORM_LCD_LCD_H
#define __CLUCK2SESAME_SRC_PLATFORM_LCD_LCD_H
#include <stdint.h>
#include "../Event.h"
#include "../Lcd.h"

#define LCD_NYBBLE_CMD 0b00000000
#define LCD_NYBBLE_DATA 0b10000000

struct LcdState
{
	uint8_t enableCount;
	union
	{
		uint8_t all;
		struct
		{
			unsigned int isBusy : 1;
		};
	} flags;

	struct
	{
		LcdTransactionCallback callback;
		void *state;
	} transaction;
};

extern void lcdOnVoltageRegulatorEnabled(const struct Event *event);
extern void lcdOnVoltageRegulatorDisabled(const struct Event *event);
extern void lcdConfigure(void);

extern void __reentrant lcdTransactionCompleted(void *unused);
extern void lcdWriteNybble(uint8_t nybble);
extern void lcdWriteCommand(uint8_t byte);
extern void lcdWriteData(uint8_t byte);

extern struct LcdState lcdState;

#endif
