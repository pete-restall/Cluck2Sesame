#include <xc.h>
#include <unity.h>

void unityBeforeRunHook(void)
{
}

void unityBreakpointHook(void)
{
	while (1);
}

void putch(unsigned char data)
{
	if (!TX2STAbits.TXEN)
	{
		TX2STAbits.TXEN = 1;
		RC2STAbits.SPEN = 1;
	}

	while (!TX2STAbits.TRMT)
		continue;

	TX2REG = data;
}
