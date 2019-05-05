#ifndef __CLUCK3SESAME_TESTS_LCD_LCDFIXTURE_H
#define __CLUCK3SESAME_TESTS_LCD_LCDFIXTURE_H

extern void fakeLcdInitialise(void);
extern void fakeLcdShutdown(void);
extern uint8_t fakeLcdIsInvalidState(void);

extern volatile uint8_t fakeLcdIsSessionInvalid;
extern volatile uint8_t fakeLcdRs;
extern volatile uint8_t fakeLcdData;
extern volatile uint8_t fakeLcdIsConfigured;
extern volatile uint8_t fakeLcdIsNybbleMode;

#endif
