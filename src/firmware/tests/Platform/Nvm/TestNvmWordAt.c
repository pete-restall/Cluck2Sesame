#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Nvm.h"

#include "../../NonDeterminism.h"
#include "../../Fixture.h"
#include "../../NvmSettingsFixture.h"

TEST_FILE("Platform/Nvm.c")
TEST_FILE("Platform/NvmSettings.c")

void onBeforeTest(void)
{
	static union NvmSettings stubbedNvmSettings;
	anyBytesInto(&stubbedNvmSettings, sizeof(union NvmSettings));
	stubNvmSettings(&stubbedNvmSettings);
}

void onAfterTest(void)
{
}

void test_nvmWordAt_calledWithAddressInProgramSpace_expectCorrectWordIsReturned(void)
{
	uint8_t offset = anyByteLessThan((uint8_t) sizeof(union NvmSettings));
	const uint8_t *ptr = ((const uint8_t *) &nvmSettings) + offset;
	uint16_t retlwWord = nvmWordAt(((uint16_t) ptr) & 0x7fff);
	TEST_ASSERT_EQUAL_HEX16(0b11010000000000 | *ptr, retlwWord);
}

// TODO: If / when the simulator supports setting NVMCON1.NVMREGS to '1', these _test* functions ought to work and
// can be renamed:

void _test_nvmWordAt_calledWithAddressInConfigurationWordSpace_expectCorrectWordIsReturned(void)
{
	static const uint16_t devid __at(0x8006);
	static const uint16_t pic16f15356 = 0x30b0;
	TEST_ASSERT_EQUAL_HEX16(pic16f15356, nvmWordAt(0x8006));
}

void _test_nvmWordAt_calledWithAddressInDeviceConfigurationInformationSpace_expectCorrectWordIsReturned(void)
{
	static const uint16_t pcnt __at(DCI_PCNT);
	TEST_ASSERT_EQUAL_HEX16(pcnt, nvmWordAt(DCI_PCNT));
}

void _test_nvmWordAt_calledWithAddressInDeviceInformationAreaSpace_expectCorrectWordIsReturned(void)
{
	static const uint16_t mui4 __at(DIA_MUI4);
	TEST_ASSERT_EQUAL_HEX16(mui4, nvmWordAt(DIA_MUI4));
}
