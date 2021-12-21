#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <unity.h>

#include "Mock_PeriodicMonitor.h"
#include "Platform/CalibrationMode.h"

#include "CalibrationModeFixtureWithUart.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"
#include "../../NvmSettingsFixture.c"

TEST_FILE("Platform/CalibrationMode.c")
TEST_FILE("Platform/Event.c")

static uint8_t anyInvalidArgument(void);

void onBeforeTest(void)
{
	calibrationModeFixtureSetUp();
	stubNvmSettingsWithCalibrationRequired();
	calibrationModeInitialise();
}

void onAfterTest(void)
{
	calibrationModeFixtureTearDown();
}

void test_uart1_receivesRefclkOffCommand_expect0IsTransmittedToHost(void)
{
	uint8_t command[] = {CALIBRATIONMODE_CMD_REFCLK, '0', CALIBRATIONMODE_CMD_EOL};
	fakeHostToDeviceSend(command, sizeof(command));
	fakeHostWaitForDeviceResponse();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(3, deviceToHostNumberOfBytes, "NUM");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_REPLY_RESULT, deviceToHostBytes[0], "0");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE('0', deviceToHostBytes[1], "1");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_CMD_EOL, deviceToHostBytes[2], "EOL");
}

void test_uart1_receivesRefclkOffCommand_expectRefclkIsDisabled(void)
{
	uint8_t originalClkrcon = CLKRCON;
	CLKRCON |= _CLKRCON_CLKREN_MASK;
	uint8_t command[] = {CALIBRATIONMODE_CMD_REFCLK, '0', CALIBRATIONMODE_CMD_EOL};
	fakeHostToDeviceSend(command, sizeof(command));
	fakeHostWaitForDeviceResponse();
	TEST_ASSERT_EQUAL_UINT8(originalClkrcon & ~_CLKRCON_CLKREN_MASK, CLKRCON);
}

void test_uart1_receivesRefclkOnCommand_expect1IsTransmittedToHost(void)
{
	uint8_t command[] = {CALIBRATIONMODE_CMD_REFCLK, '1', CALIBRATIONMODE_CMD_EOL};
	fakeHostToDeviceSend(command, sizeof(command));
	fakeHostWaitForDeviceResponse();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(3, deviceToHostNumberOfBytes, "NUM");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_REPLY_RESULT, deviceToHostBytes[0], "0");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE('1', deviceToHostBytes[1], "1");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_CMD_EOL, deviceToHostBytes[2], "EOL");
}

void test_uart1_receivesRefclkOnCommand_expectRefclkIsDisabled(void)
{
	uint8_t originalClkrcon = CLKRCON;
	CLKRCON &= ~_CLKRCON_CLKREN_MASK;
	uint8_t command[] = {CALIBRATIONMODE_CMD_REFCLK, '1', CALIBRATIONMODE_CMD_EOL};
	fakeHostToDeviceSend(command, sizeof(command));
	fakeHostWaitForDeviceResponse();
	TEST_ASSERT_EQUAL_UINT8(originalClkrcon | _CLKRCON_CLKREN_MASK, CLKRCON);
}

void test_uart1_receivesRefclkCommandWithInvalidArgument_expectErrorIsTransmittedToHost(void)
{
	uint8_t command[] = {CALIBRATIONMODE_CMD_REFCLK, anyInvalidArgument(), CALIBRATIONMODE_CMD_EOL};
	uart1_receivesInvalidCommand_expectInvalidArgumentErrorIsTransmittedToHost(command, sizeof(command));
}

static uint8_t anyInvalidArgument(void)
{
	while (true)
	{
		uint8_t arg = anyByte();
		if (arg != '0' && arg != '1' && arg != CALIBRATIONMODE_CMD_EOL)
			return arg;
	}
}

void test_uart1_receivesRefclkCommandWithoutArgument_expectErrorIsTransmittedToHost(void)
{
	uint8_t command[] = {CALIBRATIONMODE_CMD_REFCLK, CALIBRATIONMODE_CMD_EOL};
	uart1_receivesInvalidCommand_expectInvalidCommandErrorIsTransmittedToHost(command, sizeof(command));
}

void test_uart1_receivesRefclkCommandWithMoreThanOneArgument_expectErrorIsTransmittedToHost(void)
{
	uint8_t command[] = {CALIBRATIONMODE_CMD_REFCLK, '1', '0', CALIBRATIONMODE_CMD_EOL};
	uart1_receivesInvalidCommand_expectInvalidCommandErrorIsTransmittedToHost(command, sizeof(command));
}
