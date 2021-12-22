#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "HexDigits.h"

static uint8_t hexDigitHigh(uint8_t value);
static uint8_t hexDigitLow(uint8_t value);
static uint8_t hexDigitToNybble(uint8_t hexDigit);

void hexDigitsForByte(uint8_t *outTwoDigits, uint8_t inByte)
{
	*(outTwoDigits++) = hexDigitHigh(inByte);
	*outTwoDigits = hexDigitLow(inByte);
}

static uint8_t hexDigitHigh(uint8_t value)
{
	return hexDigitLow((value >> 4) & 0x0f);
}

static uint8_t hexDigitLow(uint8_t value)
{
	value &= 0x0f;
	if (value > 9)
		return 'a' + (value - 10);

	return '0' + value;
}

void hexDigitsForWord(uint8_t *outFourDigits, uint16_t inWord)
{
	hexDigitsForByte(outFourDigits, (uint8_t) (inWord >> 8));
	hexDigitsForByte(outFourDigits + 2, (uint8_t) inWord);
}

bool hexDigitsToWord(uint16_t *outWord, const uint8_t *inFourDigits)
{
	if (!outWord || !inFourDigits)
		return false;

	uint16_t word = 0;
	for (uint8_t i = 0; i < 4; i++, inFourDigits++)
	{
		uint8_t nybble = hexDigitToNybble(*inFourDigits);
		if (nybble > 15)
			return false;

		word = (word << 4) | nybble;
	}

	*outWord = word;
	return true;
}

static uint8_t hexDigitToNybble(uint8_t hexDigit)
{
	if (hexDigit >= '0' && hexDigit <= '9')
		return hexDigit - '0';

	if (hexDigit >= 'a' && hexDigit <= 'f')
		return 10 + hexDigit - 'a';

	if (hexDigit >= 'A' && hexDigit <= 'F')
		return 10 + hexDigit - 'A';

	return 0xff;
}
