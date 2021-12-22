#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Mock_Nvm.h"
#include "Mock_HexDigits.h"
#include "Mock_PeriodicMonitor.h"
#include "Platform/CalibrationMode.h"

#include "CalibrationModeFixtureWithUart.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"
#include "../../NvmSettingsFixture.h"

TEST_FILE("Platform/CalibrationMode.c")
TEST_FILE("Platform/Event.c")

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

void test_uart1_receivesNoCommand_expectOkIsTransmittedToHost(void)
{
	uint8_t command[] = {CALIBRATIONMODE_CMD_EOL};
	fakeHostToDeviceSend(command, sizeof(command));
	fakeHostWaitForDeviceResponse();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(4, deviceToHostNumberOfBytes, "NUM");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_REPLY_RESULT, deviceToHostBytes[0], "0");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE('O', deviceToHostBytes[1], "1");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE('K', deviceToHostBytes[2], "2");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_CMD_EOL, deviceToHostBytes[3], "EOL");
}
