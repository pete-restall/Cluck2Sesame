#include <xc.h>
#include <stdint.h>
#include <stdlib.h>

#include "NonDeterminism.h"

int rngSeed = 0;

static void ensureSeedIsInitialised(void)
{
	static uint8_t isInitialised = 0;

	if (isInitialised)
		return;

	isInitialised = 1;
	srand(rngSeed);
}

uint8_t anyBoolean(void)
{
	return anyByte() % 2 == 0;
}

uint8_t anyByte(void)
{
	ensureSeedIsInitialised();
	return (uint8_t) rand();
}

uint8_t anyByteExcept(uint8_t except)
{
	uint8_t byte = anyByte();
	while (byte == except)
		byte = anyByte();

	return byte;
}

uint32_t anyDword(void)
{
	uint32_t dword = 0;
	int i;

	for (i = 0; i < 4; i++)
		dword |= ((uint32_t) anyByte()) << (i * 8);

	return dword;
}
