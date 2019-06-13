#ifndef __CLUCK3SESAME_SRC_PLATFORM_LCD_LCD_H
#define __CLUCK3SESAME_SRC_PLATFORM_LCD_LCD_H
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

extern void onVoltageRegulatorEnabled(const struct Event *event);
extern void lcdConfigure(void);

extern void lcdTransactionCompleted(void *unused);
extern void lcdWriteNybble(uint8_t nybble);
extern void lcdWriteCommand(uint8_t byte);
extern void lcdWriteData(uint8_t byte);

extern struct LcdState lcdState;

#endif
