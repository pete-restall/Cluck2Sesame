#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Mock_Event.h"

#include "Platform/Buttons.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Buttons.c")

void onBeforeTest(void)
{
}

void onAfterTest(void)
{
}

void test_buttonsInitialise_called_expectInterruptOnChangeModuleIsEnabled(void)
{
	PMD0 = anyByteWithMaskSet(_PMD0_IOCMD_MASK);
	uint8_t originalPmd0 = PMD0;
	buttonsInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPmd0 & ~_PMD0_IOCMD_MASK, PMD0);
}

void test_buttonsInitialise_called_expectAnalogueBehaviourIsDisabledForRa0AndRa1(void)
{
	ANSELA = anyByteWithMaskSet(_ANSELA_ANSA0_MASK | _ANSELA_ANSA1_MASK);
	uint8_t originalAnsela = ANSELA;
	buttonsInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalAnsela & ~(_ANSELA_ANSA0_MASK | _ANSELA_ANSA1_MASK), ANSELA);
}

void test_buttonsInitialise_called_expectInterruptOnChangeFlagIsCleared(void)
{
	IOCAF = anyByteWithMaskSet(_IOCAF_IOCAF0_MASK | _IOCAF_IOCAF1_MASK);
	PIR0 = anyByteWithMaskSet(_PIR0_IOCIF_MASK);
	uint8_t originalPir0 = PIR0;
	buttonsInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPir0 & ~_PIR0_IOCIF_MASK, PIR0);
}

void test_buttonsInitialise_called_expectInterruptOnChangeIsEnabled(void)
{
	PIE0 = anyByteWithMaskClear(_PIE0_IOCIE_MASK);
	uint8_t originalPie0 = PIE0;
	buttonsInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPie0 | _PIE0_IOCIE_MASK, PIE0);
}

void test_buttonsInitialise_called_expectRa0AndRa1InterruptOnNegativeEdges(void)
{
	IOCAN = anyByteWithMaskClear(_IOCAN_IOCAN0_MASK | _IOCAN_IOCAN1_MASK);
	uint8_t originalIocan = IOCAN;
	buttonsInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalIocan | (_IOCAN_IOCAN0_MASK | _IOCAN_IOCAN1_MASK), IOCAN);
}

void test_buttonsInitialise_called_expectRa0AndRa1InterruptOnPositiveEdges(void)
{
	IOCAP = anyByteWithMaskClear(_IOCAP_IOCAP0_MASK | _IOCAP_IOCAP1_MASK);
	uint8_t originalIocap = IOCAP;
	buttonsInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalIocap | (_IOCAP_IOCAP0_MASK | _IOCAP_IOCAP1_MASK), IOCAP);
}
