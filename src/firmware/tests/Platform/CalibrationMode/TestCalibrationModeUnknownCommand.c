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

static uint8_t anyUnknownCommand(void);

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

void test_uart1_receivesUnknownCommand_expectErrorWithCorrectCodeIsTransmittedToHost(void)
{
	uint8_t command[] = {anyUnknownCommand(), CALIBRATIONMODE_CMD_EOL};
	fakeHostToDeviceSend(command, sizeof(command));
	fakeHostWaitForDeviceResponse();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(4, deviceToHostNumberOfBytes, "NUM");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_REPLY_ERROR, deviceToHostBytes[0], "0");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE('0', deviceToHostBytes[1], "1");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE('1', deviceToHostBytes[2], "2");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_CMD_EOL, deviceToHostBytes[3], "EOL");
}

static uint8_t anyUnknownCommand(void)
{
	uint8_t unknownCommand;
	while (true)
	{
		unknownCommand = anyByte();
		switch (unknownCommand)
		{
			case CALIBRATIONMODE_CMD_EOL:
			case CALIBRATIONMODE_CMD_READ:
			case CALIBRATIONMODE_CMD_REFCLK:
			case CALIBRATIONMODE_CMD_SAMPLEPARAMETERS:
				break;

			default:
				return unknownCommand;
		}
	}
}
