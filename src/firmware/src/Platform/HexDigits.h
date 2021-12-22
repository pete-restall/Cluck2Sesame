#ifndef __CLUCK3SESAME_SRC_PLATFORM_HEXDIGITS_H
#define __CLUCK3SESAME_SRC_PLATFORM_HEXDIGITS_H
#include <stdint.h>
#include <stdbool.h>

extern void hexDigitsForByte(uint8_t *outTwoDigits, uint8_t inByte);
extern void hexDigitsForWord(uint8_t *outFourDigits, uint16_t inWord);
extern bool hexDigitsToWord(uint16_t *outWord, const uint8_t *inFourDigits);

#endif
