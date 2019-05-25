#include <xc.h>
#include <unity.h>

#include "NvmSettings.h"

#include "Fixture.h"

TEST_FILE("NvmSettings.c")

void onBeforeTest(void)
{
}

void onAfterTest(void)
{
}

void test_nvmSettings_getLcdContrast_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8(0x65, nvmSettings.lcd.contrast);
}

void test_nvmSettings_getLcdBacklightBrightness_expectDefaultValue(void)
{
	TEST_ASSERT_EQUAL_UINT8(0x80, nvmSettings.lcd.backlightBrightness);
}
