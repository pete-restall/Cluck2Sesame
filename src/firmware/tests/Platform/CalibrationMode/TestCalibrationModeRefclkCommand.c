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

static uint8_t anyUnknownArgument(void);

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

void test_uart1_receivesRefclkCommandWithUnknownArgument_expectErrorIsTransmittedToHost(void)
{
	uint8_t command[] = {CALIBRATIONMODE_CMD_REFCLK, anyUnknownArgument(), CALIBRATIONMODE_CMD_EOL};
	fakeHostToDeviceSend(command, sizeof(command));
	fakeHostWaitForDeviceResponse();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(4, deviceToHostNumberOfBytes, "NUM");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_REPLY_ERROR, deviceToHostBytes[0], "0");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE('0', deviceToHostBytes[1], "1");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE('2', deviceToHostBytes[2], "2");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_CMD_EOL, deviceToHostBytes[3], "EOL");
}

static uint8_t anyUnknownArgument(void)
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
	fakeHostToDeviceSend(command, sizeof(command));
	fakeHostWaitForDeviceResponse();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(4, deviceToHostNumberOfBytes, "NUM");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_REPLY_ERROR, deviceToHostBytes[0], "0");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE('0', deviceToHostBytes[1], "1");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE('1', deviceToHostBytes[2], "2");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_CMD_EOL, deviceToHostBytes[3], "EOL");
}

void test_uart1_receivesRefclkCommandWithMoreThanOneArgument_expectErrorIsTransmittedToHost(void)
{
	uint8_t command[] = {CALIBRATIONMODE_CMD_REFCLK, '1', '0', CALIBRATIONMODE_CMD_EOL};
	fakeHostToDeviceSend(command, sizeof(command));
	fakeHostWaitForDeviceResponse();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(4, deviceToHostNumberOfBytes, "NUM");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_REPLY_ERROR, deviceToHostBytes[0], "0");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE('0', deviceToHostBytes[1], "1");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE('1', deviceToHostBytes[2], "2");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_CMD_EOL, deviceToHostBytes[3], "EOL");
}
