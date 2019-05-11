#include <xc.h>
#include <unity.h>

#include "Lcd.h"

#include "LcdFixture.h"
#include "NonDeterminism.h"

#define DRAM_ADDR(x) __at(0x23a0 + (x))

volatile uint8_t fakeLcdRs;
volatile uint8_t fakeLcdData;
volatile uint8_t fakeLcdInitialFunctionSetCount;
volatile uint8_t fakeLcdIsConfigured;
volatile uint8_t fakeLcdIsNybbleMode;
volatile uint8_t fakeLcdClocked;
volatile uint8_t fakeLcdCommand;
volatile uint8_t fakeLcdBusyFlag;
volatile uint32_t fakeLcdCommandIndex;

volatile uint8_t fakeLcdSessionIndex;
volatile uint8_t fakeLcdIsSessionInvalid;

volatile uint8_t fakeLcdRegFunction;
volatile uint8_t fakeLcdRegDisplay;
volatile uint8_t fakeLcdRegEntryMode;
volatile uint8_t fakeLcdRegDdramAddress;

volatile uint8_t fakeLcdDram[16 * 2] DRAM_ADDR(0);
volatile uint8_t fakeLcdDram0_0 DRAM_ADDR(0);
volatile uint8_t fakeLcdDram0_1 DRAM_ADDR(1);
volatile uint8_t fakeLcdDram0_2 DRAM_ADDR(2);
volatile uint8_t fakeLcdDram0_3 DRAM_ADDR(3);
volatile uint8_t fakeLcdDram0_4 DRAM_ADDR(4);
volatile uint8_t fakeLcdDram0_5 DRAM_ADDR(5);
volatile uint8_t fakeLcdDram0_6 DRAM_ADDR(6);
volatile uint8_t fakeLcdDram0_7 DRAM_ADDR(7);
volatile uint8_t fakeLcdDram0_8 DRAM_ADDR(8);
volatile uint8_t fakeLcdDram0_9 DRAM_ADDR(9);
volatile uint8_t fakeLcdDram0_10 DRAM_ADDR(10);
volatile uint8_t fakeLcdDram0_11 DRAM_ADDR(11);
volatile uint8_t fakeLcdDram0_12 DRAM_ADDR(12);
volatile uint8_t fakeLcdDram0_13 DRAM_ADDR(13);
volatile uint8_t fakeLcdDram0_14 DRAM_ADDR(14);
volatile uint8_t fakeLcdDram0_15 DRAM_ADDR(15);
volatile uint8_t fakeLcdDram1_0 DRAM_ADDR(16);
volatile uint8_t fakeLcdDram1_1 DRAM_ADDR(17);
volatile uint8_t fakeLcdDram1_2 DRAM_ADDR(18);
volatile uint8_t fakeLcdDram1_3 DRAM_ADDR(19);
volatile uint8_t fakeLcdDram1_4 DRAM_ADDR(20);
volatile uint8_t fakeLcdDram1_5 DRAM_ADDR(21);
volatile uint8_t fakeLcdDram1_6 DRAM_ADDR(22);
volatile uint8_t fakeLcdDram1_7 DRAM_ADDR(23);
volatile uint8_t fakeLcdDram1_8 DRAM_ADDR(24);
volatile uint8_t fakeLcdDram1_9 DRAM_ADDR(25);
volatile uint8_t fakeLcdDram1_10 DRAM_ADDR(26);
volatile uint8_t fakeLcdDram1_11 DRAM_ADDR(27);
volatile uint8_t fakeLcdDram1_12 DRAM_ADDR(28);
volatile uint8_t fakeLcdDram1_13 DRAM_ADDR(29);
volatile uint8_t fakeLcdDram1_14 DRAM_ADDR(30);
volatile uint8_t fakeLcdDram1_15 DRAM_ADDR(31);

void fakeLcdInitialise(void)
{
	for (uint8_t i = 0; i < sizeof(fakeLcdDram); i++)
		fakeLcdDram[i] = i;

	fakeLcdSessionIndex++;
}

void fakeLcdShutdown(void)
{
	TEST_ASSERT_FALSE_MESSAGE(fakeLcdIsSessionInvalid, "LCD violations !");
}

void fakeLcdAssertFunctionRegister(uint8_t flags)
{
	TEST_ASSERT_EQUAL_HEX8(LCD_CMD_FUNCTION | flags, fakeLcdRegFunction);
}

void fakeLcdAssertDisplayRegister(uint8_t flags)
{
	TEST_ASSERT_EQUAL_HEX8(LCD_CMD_DISPLAY | flags, fakeLcdRegDisplay);
}

void fakeLcdAssertEntryModeRegister(uint8_t flags)
{
	TEST_ASSERT_EQUAL_HEX8(LCD_CMD_ENTRYMODE | flags, fakeLcdRegEntryMode);
}

void fakeLcdAssertDdramAddressRegisterIs(uint8_t address)
{
	TEST_ASSERT_EQUAL_HEX8(address, fakeLcdRegDdramAddress);
}
