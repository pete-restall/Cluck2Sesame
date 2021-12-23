#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <unity.h>

#include "Mock_Nvm.h"
#include "Platform/HexDigits.h"
#include "Mock_PeriodicMonitor.h"
#include "Platform/CalibrationMode.h"

#include "CalibrationModeFixtureWithUart.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"
#include "../../NvmSettingsFixture.h"

TEST_FILE("Platform/CalibrationMode.c")
TEST_FILE("Platform/Event.c")

static void periodicMonitorSampleNowCallback(struct MonitoredParametersSampled *sample, int numCalls);
static void stubMonitoredParametersSampledFor(const struct MonitoredParametersSampled *sample);
static uint8_t hexDigitHigh(uint8_t value);
static uint8_t hexDigitLow(uint8_t value);
static uint8_t byteHigh(uint16_t value);
static uint8_t byteLow(uint16_t value);

static const struct MonitoredParametersSampled *stubbedMonitoredParametersSampled;

void onBeforeTest(void)
{
	calibrationModeFixtureSetUp();
	stubNvmSettingsWithCalibrationRequired();
	calibrationModeInitialise();
	periodicMonitorSampleNow_StubWithCallback(&periodicMonitorSampleNowCallback);
}

static void periodicMonitorSampleNowCallback(struct MonitoredParametersSampled *sample, int numCalls)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(stubbedMonitoredParametersSampled, "A");
	TEST_ASSERT_NOT_NULL_MESSAGE(sample, "B");
	memcpy(sample, stubbedMonitoredParametersSampled, sizeof(struct MonitoredParametersSampled));
}

void onAfterTest(void)
{
	calibrationModeFixtureTearDown();
}

void test_uart1_receivesSampleParametersCommand_expectSampledParametersAreTransmittedToHost(void)
{
	struct MonitoredParametersSampled sample;
	stubMonitoredParametersSampledFor(anyBytesInto(&sample, sizeof(struct MonitoredParametersSampled)));
	uint8_t command[] = {CALIBRATIONMODE_CMD_SAMPLEPARAMETERS, CALIBRATIONMODE_CMD_EOL};
	fakeHostToDeviceSend(command, sizeof(command));
	fakeHostWaitForDeviceResponse();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(17, deviceToHostNumberOfBytes, "NUM");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_REPLY_RESULT, deviceToHostBytes[0], "0");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(hexDigitHigh(sample.timestamp), deviceToHostBytes[1], "1");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(hexDigitLow(sample.timestamp), deviceToHostBytes[2], "2");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(',', deviceToHostBytes[3], "3");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(hexDigitHigh(sample.flags.all), deviceToHostBytes[4], "4");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(hexDigitLow(sample.flags.all), deviceToHostBytes[5], "5");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(',', deviceToHostBytes[6], "6");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(hexDigitHigh(byteHigh(sample.fvr)), deviceToHostBytes[7], "7");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(hexDigitLow(byteHigh(sample.fvr)), deviceToHostBytes[8], "8");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(hexDigitHigh(byteLow(sample.fvr)), deviceToHostBytes[9], "9");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(hexDigitLow(byteLow(sample.fvr)), deviceToHostBytes[10], "10");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(',', deviceToHostBytes[11], "11");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(hexDigitHigh(byteHigh(sample.temperature)), deviceToHostBytes[12], "12");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(hexDigitLow(byteHigh(sample.temperature)), deviceToHostBytes[13], "13");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(hexDigitHigh(byteLow(sample.temperature)), deviceToHostBytes[14], "14");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(hexDigitLow(byteLow(sample.temperature)), deviceToHostBytes[15], "15");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(CALIBRATIONMODE_CMD_EOL, deviceToHostBytes[16], "EOL");
}

static void stubMonitoredParametersSampledFor(const struct MonitoredParametersSampled *sample)
{
	stubbedMonitoredParametersSampled = sample;
}

static uint8_t hexDigitHigh(uint8_t value)
{
	return hexDigitLow((value >> 4) & 0x0f);
}

static uint8_t hexDigitLow(uint8_t value)
{
	value &= 0x0f;
	if (value > 9)
		return 'a' + (value - 10);

	return '0' + value;
}

static uint8_t byteHigh(uint16_t value)
{
	return (uint8_t) (value >> 8);
}

static uint8_t byteLow(uint16_t value)
{
	return (uint8_t) (value & 0xff);
}

void test_uart1_receivesSampleParametersCommandWithAnArgument_expectErrorIsTransmittedToHost(void)
{
	uint8_t command[] = {CALIBRATIONMODE_CMD_SAMPLEPARAMETERS, '1', CALIBRATIONMODE_CMD_EOL};
	uart1_receivesInvalidCommand_expectInvalidCommandErrorIsTransmittedToHost(command, sizeof(command));
}
