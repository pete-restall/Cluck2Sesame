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

void test_uart1_receivesInvalidCommand_expectErrorWithCorrectCodeIsTransmittedToHost(void)
{
	uint8_t command[] = {anyUnknownCommand(), CALIBRATIONMODE_CMD_EOL};
	uart1_receivesInvalidCommand_expectInvalidCommandErrorIsTransmittedToHost(command, sizeof(command));
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
