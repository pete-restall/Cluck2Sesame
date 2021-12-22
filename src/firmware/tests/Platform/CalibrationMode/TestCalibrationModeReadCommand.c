#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <unity.h>

#include "Platform/HexDigits.h"
#include "Mock_PeriodicMonitor.h"
#include "Mock_Nvm.h"
#include "Platform/CalibrationMode.h"

#include "CalibrationModeFixtureWithUart.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"
#include "../../NvmSettingsFixture.h"

TEST_FILE("Platform/CalibrationMode.c")
TEST_FILE("Platform/Event.c")

static uint8_t anyNonHexDigit(void);

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

void test_uart1_receivesReadCommandWithAddress_expectNvmWordAtAddressIsTransmittedToHost(void)
{
	uint16_t address = anyWord();
	uint16_t wordAtAddress = anyWord();
	uint8_t wordAtAddressHexDigits[4];
	hexDigitsForWord(wordAtAddressHexDigits, wordAtAddress);
	nvmWordAt_ExpectAndReturn(address, wordAtAddress);

	uint8_t command[] = {CALIBRATIONMODE_CMD_READ, 'x', 'x', 'x', 'x', CALIBRATIONMODE_CMD_EOL};
	hexDigitsForWord(&command[1], address);
	fakeHostToDeviceSend(command, sizeof(command));
	fakeHostWaitForDeviceResponse();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(6, deviceToHostNumberOfBytes, "NUM");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_REPLY_RESULT, deviceToHostBytes[0], "0");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(wordAtAddressHexDigits[0], deviceToHostBytes[1], "1");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(wordAtAddressHexDigits[1], deviceToHostBytes[2], "2");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(wordAtAddressHexDigits[2], deviceToHostBytes[3], "3");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(wordAtAddressHexDigits[3], deviceToHostBytes[4], "4");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_CMD_EOL, deviceToHostBytes[5], "EOL");
}

void test_uart1_receivesReadCommandWithNonHexDigit_expectErrorIsTransmittedToHost(void)
{
	uint8_t command[] = {CALIBRATIONMODE_CMD_READ, 'x', 'x', 'x', 'x', CALIBRATIONMODE_CMD_EOL};
	hexDigitsForWord(&command[1], anyWord());
	uint8_t nonHexIndex = 1 + anyByteLessThan(4);
	command[nonHexIndex] = anyNonHexDigit();
	uart1_receivesInvalidCommand_expectInvalidArgumentErrorIsTransmittedToHost(command, sizeof(command));
}

static uint8_t anyNonHexDigit(void)
{
	while (true)
	{
		uint8_t nonHexDigit = anyByte();
		if (nonHexDigit < '0' || nonHexDigit > 'f')
			return nonHexDigit;

		if (nonHexDigit > '9' && nonHexDigit < 'A')
			return nonHexDigit;

		if (nonHexDigit > 'F' && nonHexDigit < 'a')
			return nonHexDigit;
	}
}

void test_uart1_receivesReadCommandWithoutArgument_expectErrorIsTransmittedToHost(void)
{
	uint8_t command[] = {CALIBRATIONMODE_CMD_READ, CALIBRATIONMODE_CMD_EOL};
	uart1_receivesInvalidCommand_expectInvalidCommandErrorIsTransmittedToHost(command, sizeof(command));
}

void test_uart1_receivesRefclkCommandWithMoreThanOneFourByteArgument_expectErrorIsTransmittedToHost(void)
{
	uint8_t command[] = {CALIBRATIONMODE_CMD_READ, '1', '2', '3', '4', '5', CALIBRATIONMODE_CMD_EOL};
	uart1_receivesInvalidCommand_expectInvalidCommandErrorIsTransmittedToHost(command, sizeof(command));
}
