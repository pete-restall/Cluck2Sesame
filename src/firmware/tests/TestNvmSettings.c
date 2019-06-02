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

void test_nvmSettings_getMotorCurrentLimit_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8(0x13, nvmSettings.platform.motor.currentLimit);
}

void test_nvmSettings_getLatitudeOffset_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8(0, nvmSettings.application.location.latitudeOffset);
}

void test_nvmSettings_getLongitudeOffset_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8(0, nvmSettings.application.location.longitudeOffset);
}

void test_nvmSettings_getDoorHeight_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8(0, nvmSettings.application.door.height);
}
