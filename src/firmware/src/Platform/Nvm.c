#include <xc.h>
#include <stdint.h>

uint16_t nvmWordAt(uint16_t address)
{
	NVMCON1bits.NVMREGS = (address & 0x8000) ? 1 : 0;
	NVMADR = address;
	NVMCON1bits.RD = 1;
	asm("nop");
	return NVMDAT;
}
