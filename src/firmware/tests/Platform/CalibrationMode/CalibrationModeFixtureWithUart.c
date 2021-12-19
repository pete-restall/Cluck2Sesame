#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/NvmSettings.h"
#include "Platform/PowerManagement.h"
#include "Platform/PeriodicMonitor.h"
#include "Platform/CalibrationMode.h"

#include "../../NonDeterminism.h"
#include "../../Fixture.h"
#include "../../NvmSettingsFixture.h"

#include "CalibrationModeFixtureWithUart.h"

#define HOST_TO_DEVICE_BYTE_ADDR(x) __at(0x23a0 + (x))
#define DEVICE_TO_HOST_BYTE_ADDR(x) __at(0x23a8 + (x))

static void fakeUart1Initialise(void);

volatile uint8_t hostToDeviceBytes[4] HOST_TO_DEVICE_BYTE_ADDR(0);
volatile uint8_t hostToDeviceByte0 HOST_TO_DEVICE_BYTE_ADDR(0);
volatile uint8_t hostToDeviceByte1 HOST_TO_DEVICE_BYTE_ADDR(1);
volatile uint8_t hostToDeviceByte2 HOST_TO_DEVICE_BYTE_ADDR(2);
volatile uint8_t hostToDeviceByte3 HOST_TO_DEVICE_BYTE_ADDR(3);
volatile uint8_t hostToDeviceNumberOfBytes;

volatile uint8_t deviceToHostBytes[18] DEVICE_TO_HOST_BYTE_ADDR(0);
volatile uint8_t deviceToHostByte0 DEVICE_TO_HOST_BYTE_ADDR(0);
volatile uint8_t deviceToHostByte1 DEVICE_TO_HOST_BYTE_ADDR(1);
volatile uint8_t deviceToHostByte2 DEVICE_TO_HOST_BYTE_ADDR(2);
volatile uint8_t deviceToHostByte3 DEVICE_TO_HOST_BYTE_ADDR(3);
volatile uint8_t deviceToHostByte4 DEVICE_TO_HOST_BYTE_ADDR(4);
volatile uint8_t deviceToHostByte5 DEVICE_TO_HOST_BYTE_ADDR(5);
volatile uint8_t deviceToHostByte6 DEVICE_TO_HOST_BYTE_ADDR(6);
volatile uint8_t deviceToHostByte7 DEVICE_TO_HOST_BYTE_ADDR(7);
volatile uint8_t deviceToHostByte8 DEVICE_TO_HOST_BYTE_ADDR(8);
volatile uint8_t deviceToHostByte9 DEVICE_TO_HOST_BYTE_ADDR(9);
volatile uint8_t deviceToHostByte10 DEVICE_TO_HOST_BYTE_ADDR(10);
volatile uint8_t deviceToHostByte11 DEVICE_TO_HOST_BYTE_ADDR(11);
volatile uint8_t deviceToHostByte12 DEVICE_TO_HOST_BYTE_ADDR(12);
volatile uint8_t deviceToHostByte13 DEVICE_TO_HOST_BYTE_ADDR(13);
volatile uint8_t deviceToHostByte14 DEVICE_TO_HOST_BYTE_ADDR(14);
volatile uint8_t deviceToHostByte15 DEVICE_TO_HOST_BYTE_ADDR(15);
volatile uint8_t deviceToHostByte16 DEVICE_TO_HOST_BYTE_ADDR(16);
volatile uint8_t deviceToHostByte17 DEVICE_TO_HOST_BYTE_ADDR(17);
volatile uint8_t deviceToHostNumberOfBytes;

volatile uint8_t fakeUart1IsSessionInvalid;

void calibrationModeFixtureSetUp(void)
{
	fakeUart1Initialise();
	eventInitialise();
}

static void fakeUart1Initialise(void)
{
	deviceToHostNumberOfBytes = 0;
	for (uint8_t i = 0; i < sizeof(deviceToHostBytes); i++)
		deviceToHostBytes[i] = i;

	hostToDeviceNumberOfBytes = 0;
	for (uint8_t i = 0; i < sizeof(hostToDeviceBytes); i++)
		hostToDeviceBytes[i] = i;

	fakeUart1IsSessionInvalid = 0;
}

void calibrationModeFixtureTearDown(void)
{
	TEST_ASSERT_FALSE_MESSAGE(fakeUart1IsSessionInvalid, "UART1 violations !");
}

void stubNvmSettingsWithCalibrationRequired(void)
{
	static const union NvmSettings withCalibrationRequired =
	{
		.platform =
		{
			.flags = { .bits = { .isCalibrationRequired = 1 } }
		}
	};

	stubNvmSettings(&withCalibrationRequired);
}

void stubNvmSettingsWithoutCalibrationRequired(void)
{
	static const union NvmSettings withoutCalibrationRequired =
	{
		.platform =
		{
			.flags = { .bits = { .isCalibrationRequired = 0 } }
		}
	};

	stubNvmSettings(&withoutCalibrationRequired);
}

void fakeHostToDeviceSend(const uint8_t *bytes, size_t numberOfBytes)
{
	if (numberOfBytes > sizeof(hostToDeviceBytes))
		numberOfBytes = sizeof(hostToDeviceBytes);

	memcpy((void *) hostToDeviceBytes, bytes, numberOfBytes);
	hostToDeviceNumberOfBytes = (uint8_t) numberOfBytes;
}

void fakeHostWaitForDeviceResponse(void)
{
	CPUDOZEbits.DOZEN = 0;
	CPUDOZEbits.IDLEN = 1;
	INTCONbits.PEIE = 1;

	uint8_t byteIndex = 0;
	while (true)
	{
		dispatchAllEvents();
		eventPublish(WOKEN_FROM_SLEEP, &eventEmptyArgs);

		while (byteIndex < deviceToHostNumberOfBytes)
		{
			TEST_ASSERT_LESS_THAN_MESSAGE(sizeof(deviceToHostBytes), byteIndex, "Receive overflow");
			if (deviceToHostBytes[byteIndex] == CALIBRATIONMODE_CMD_EOL)
			{
				dispatchAllEvents();
				return;
			}

			byteIndex++;
		}
	}
}
