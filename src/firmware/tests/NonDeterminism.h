#ifndef __CLUCK2SESAME_TESTS_NONDETERMINISM_H
#define __CLUCK2SESAME_TESTS_NONDETERMINISM_H
#include <stdint.h>

extern uint8_t anyByte(void);
extern uint8_t anyByteExcept(uint8_t except);
extern uint8_t anyByteWithMaskSet(uint8_t mask);
extern uint8_t anyByteWithMaskClear(uint8_t mask);
extern uint8_t anyByteLessThan(uint8_t value);
extern uint8_t anyBoolean(void);
extern uint16_t anyWord(void);
extern uint16_t anyWordExcept(uint16_t except);
extern uint16_t anyWordLessThan(uint16_t value);
extern void *anyBytesInto(void *dest, size_t numberOfBytes);

#endif
