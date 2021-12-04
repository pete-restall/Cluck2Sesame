#include <xc.h>
#include <unity.h>

#include "Platform/NvmSettings.h"

#include "Fixture.h"

TEST_FILE("Platform/NvmSettings.c")

void onBeforeTest(void)
{
}

void onAfterTest(void)
{
}

void test_nvmSettings_getLcdContrast_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8(0x65, nvmSettings.platform.lcd.contrast);
}

void test_nvmSettings_getLcdBacklightBrightness_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8(0x80, nvmSettings.platform.lcd.backlightBrightness);
}

void test_nvmSettings_getMotorCurrentLimitNoLoad_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8(0x07, nvmSettings.platform.motor.currentLimitNoLoad);
}

void test_nvmSettings_getMotorCurrentLimitMaximumLoad_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8(0x10, nvmSettings.platform.motor.currentLimitMaximumLoad);
}

void test_nvmSettings_getIsCalibrationRequired_expectTrue(void)
{
	TEST_ASSERT_TRUE(nvmSettings.platform.flags.bits.isCalibrationRequired);
}

void test_nvmSettings_getLatitudeOffset_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8(0, nvmSettings.application.location.latitudeOffset);
}

void test_nvmSettings_getLongitudeOffset_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8(0, nvmSettings.application.location.longitudeOffset);
}

void test_nvmSettings_getDoorAutoOpenTime_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		0,
		nvmSettings.application.door.autoOpenTime.hour,
		"Hour");

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		0,
		nvmSettings.application.door.autoOpenTime.minute,
		"Minute");
}

void test_nvmSettings_getDoorAutoCloseTime_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		0,
		nvmSettings.application.door.autoCloseTime.hour,
		"Hour");

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		0,
		nvmSettings.application.door.autoCloseTime.minute,
		"Minute");
}

void test_nvmSettings_getSunEventOffsets_expectDefaultValues(void)
{
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		0,
		nvmSettings.application.door.sunEvents.sunriseOffsetMinutes,
		"Sunrise");

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(
		0,
		nvmSettings.application.door.sunEvents.sunsetOffsetMinutes,
		"Sunset");
}

void test_nvmSettings_getDoorHeight_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT16(0, nvmSettings.application.door.height);
}

void test_nvmSettings_getScreenTimeoutSeconds_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8(20, nvmSettings.application.ui.screenTimeoutSeconds);
}
