#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Lcd.h"

#include "FakeLcd.h"
#include "LcdFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Poll.c")
TEST_FILE("Platform/Event.c")
TEST_FILE("Platform/NearScheduler.c")
TEST_FILE("Platform/PowerManagement.c")
TEST_FILE("Platform/PwmTimer.c")
TEST_FILE("Platform/Clock/ClockInitialise.c")
TEST_FILE("Platform/Clock/ClockGetSetNow.c")
TEST_FILE("Platform/Lcd/LcdInitialise.c")
TEST_FILE("Platform/Lcd/LcdEnableDisable.c")
TEST_FILE("Platform/Lcd/LcdConfigure.c")
TEST_FILE("Platform/Lcd/LcdWrite.c")

extern void poll(void);

void onBeforeTest(void)
{
	lcdFixtureInitialise();
}

void onAfterTest(void)
{
	lcdFixtureShutdown();
}

void test_lcdConfigure_called_expectFirstByteSentToLcdIsFunctionSetForByteMode(void)
{
	lcdEnable();
	while (!fakeLcdIsSessionInvalid && fakeLcdData == 0)
		poll();

	TEST_ASSERT_FALSE_MESSAGE(fakeLcdRs, "RS");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0b00110000, fakeLcdData, "DATA");
}

void test_lcdConfigure_called_expectLcdIsConfiguredInNybbleMode(void)
{
	enableLcdAndWaitUntilDone();
	TEST_ASSERT_TRUE(fakeLcdIsNybbleMode);
}

void test_lcdConfigure_called_expectLcdContrastPwmIsEnabled(void)
{
	enableLcdAndWaitUntilDone();
	TEST_ASSERT_TRUE(PWM5CONbits.PWM5EN);
}

void test_lcdConfigure_called_expectFunctionSetForTwoLinesAnd5x8Font(void)
{
	enableLcdAndWaitUntilDone();
	fakeLcdAssertFunctionRegister(
		LCD_CMD_FUNCTION_TWOLINES | LCD_CMD_FUNCTION_FONT5X8);
}

void test_lcdConfigure_called_expectDisplayIsCleared(void)
{
	enableLcdAndWaitUntilDone();
	for (uint8_t i = 0; i < 32; i++)
	{
		TEST_ASSERT_EQUAL_HEX8(' ', fakeLcdDram[i]);
	}
}

void test_lcdConfigure_called_expectDdramAddressIsZero(void)
{
	enableLcdAndWaitUntilDone();
	fakeLcdAssertDdramAddressRegisterIs(0);
}

void test_lcdConfigure_called_expectEntryModeSetForIncrementingAddressesAndNoShifting(void)
{
	enableLcdAndWaitUntilDone();
	fakeLcdAssertEntryModeRegister(
		LCD_CMD_ENTRYMODE_INCREMENT | LCD_CMD_ENTRYMODE_NOSHIFT);
}

void test_lcdConfigure_called_expectDisplayIsOnWithCursorAndBlinkingBothOff(void)
{
	enableLcdAndWaitUntilDone();
	fakeLcdAssertDisplayRegister(LCD_CMD_DISPLAY_ON);
}
