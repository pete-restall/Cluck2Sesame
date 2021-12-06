#include <xc.h>
#include <stdint.h>

#include "NonDeterminism.h"

volatile int16_t rngSeed __at(0x0420);

static void ensureSeedIsInitialised(void)
{
	static uint8_t isInitialised = 0;
	if (isInitialised)
		return;

	isInitialised = 1;
	if (rngSeed == 0)
		rngSeed = 1;

	rngSeed = rngSeed;
}

uint8_t anyBoolean(void)
{
	return anyByte() % 2 == 0;
}

uint8_t anyByte(void)
{
	ensureSeedIsInitialised();

	uint8_t msb = rngSeed < 0 ? 1 : 0;
	rngSeed <<= 1;
	if (msb)
		rngSeed ^= 0x002d;

	return (uint8_t) rngSeed;
}

uint8_t anyByteExcept(uint8_t except)
{
	uint8_t byte = anyByte();
	while (byte == except)
		byte = anyByte();

	return byte;
}

uint8_t anyByteWithMaskSet(uint8_t mask)
{
	return anyByte() | mask;
}

uint8_t anyByteWithMaskClear(uint8_t mask)
{
	return anyByte() & ~mask;
}

uint8_t anyByteLessThan(uint8_t value)
{
	return anyByte() % value;
}

uint16_t anyWordExcept(uint16_t except)
{
	uint16_t word = anyWord();
	while (word == except)
		word = anyWord();

	return word;
}

uint16_t anyWord(void)
{
	uint16_t word = 0;
	for (uint8_t i = 0; i < 2; i++)
		word |= ((uint16_t) anyByte()) << (i * 8);

	return word;
}

uint16_t anyWordLessThan(uint16_t value)
{
	return anyWord() % value;
}
