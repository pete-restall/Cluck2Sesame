#include <xc.h>
#include <stdint.h>

#include "Lcd.h"

void lcdWriteCommand(uint8_t byte)
{
	lcdWriteNybble(LCD_NYBBLE_CMD | ((byte >> 4) & 0b00001111));
	lcdWriteNybble(LCD_NYBBLE_CMD | ((byte >> 0) & 0b00001111));
}

void lcdWriteNybble(uint8_t nybble)
{
	if (lcdState.enableCount == 0)
		return;

	LATAbits.LATA4 = 1;
	LATAbits.LATA3 = (nybble & 0b10000000) ? 1 : 0;
	LATCbits.LATC4 = (nybble & 0b00001000) ? 1 : 0;
	LATAbits.LATA6 = (nybble & 0b00000100) ? 1 : 0;
	LATAbits.LATA7 = (nybble & 0b00000010) ? 1 : 0;
	LATAbits.LATA5 = (nybble & 0b00000001) ? 1 : 0;
	LATAbits.LATA4 = 0;
}

void lcdWriteData(uint8_t byte)
{
	lcdWriteNybble(LCD_NYBBLE_DATA | ((byte >> 4) & 0b00001111));
	lcdWriteNybble(LCD_NYBBLE_DATA | ((byte >> 0) & 0b00001111));
}
