#ifndef __CLUCK3SESAME_SRC_LCD_LCD_H
#define __CLUCK3SESAME_SRC_LCD_LCD_H
#include "../Event.h"
#include "../Lcd.h"

extern void onVoltageRegulatorEnabled(const struct Event *event);
extern void lcdConfigure(void);

extern uint8_t lcdEnableCount;

#endif
