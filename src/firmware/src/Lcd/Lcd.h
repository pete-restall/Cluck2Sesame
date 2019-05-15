#ifndef __CLUCK3SESAME_SRC_LCD_LCD_H
#define __CLUCK3SESAME_SRC_LCD_LCD_H
#include "../Event.h"
#include "../Lcd.h"

#define LCD_NYBBLE_CMD 0b00000000
#define LCD_NYBBLE_DATA 0b10000000

extern void onVoltageRegulatorEnabled(const struct Event *event);
extern void lcdConfigure(void);

extern void lcdWriteNybble(uint8_t nybble);
extern void lcdWriteCommand(uint8_t byte);
extern void lcdWriteData(uint8_t byte);

extern uint8_t lcdEnableCount;

#endif
