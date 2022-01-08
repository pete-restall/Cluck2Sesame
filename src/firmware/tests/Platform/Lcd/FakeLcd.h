#ifndef __CLUCK2SESAME_TESTS_PLATFORM_LCD_FAKELCD_H
#define __CLUCK2SESAME_TESTS_PLATFORM_LCD_FAKELCD_H
#include <stdint.h>

extern void fakeLcdInitialise(void);
extern void fakeLcdShutdown(void);
extern void fakeLcdAssertStateIsValid(void);
extern void fakeLcdAssertNotBusy(void);
extern void fakeLcdAssertFunctionRegister(uint8_t flags);
extern void fakeLcdAssertDisplayRegister(uint8_t flags);
extern void fakeLcdAssertEntryModeRegister(uint8_t flags);
extern void fakeLcdAssertDdramAddressRegisterIs(uint8_t address);

extern volatile uint8_t fakeLcdIsSessionInvalid;
extern volatile uint8_t fakeLcdRs;
extern volatile uint8_t fakeLcdData;
extern volatile uint8_t fakeLcdIsConfigured;
extern volatile uint8_t fakeLcdIsNybbleMode;
extern volatile uint8_t fakeLcdDram[16 * 2];

#endif
