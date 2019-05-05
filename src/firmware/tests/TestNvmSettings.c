#include <xc.h>
#include <unity.h>

#include "NvmSettings.h"

TEST_FILE("NvmSettings.c")

void setUp(void)
{
}

void tearDown(void)
{
}

void test_nvmSettings_getLcdContrast_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8(0x26, nvmSettings.lcd.contrast);
}

void test_nvmSettings_getLcdBacklightBrightness_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8(0x80, nvmSettings.lcd.backlightBrightness);
}
