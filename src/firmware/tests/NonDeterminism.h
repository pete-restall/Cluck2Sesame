#ifndef __CLUCK3SESAME_TESTS_NONDETERMINISM_H
#define __CLUCK3SESAME_TESTS_NONDETERMINISM_H
#include <stdint.h>

extern uint8_t anyByte(void);
extern uint8_t anyByteExcept(uint8_t except);
extern uint8_t anyByteWithMaskSet(uint8_t mask);
extern uint8_t anyByteWithMaskClear(uint8_t mask);
extern uint8_t anyBoolean(void);
extern uint16_t anyWord(void);

#endif
