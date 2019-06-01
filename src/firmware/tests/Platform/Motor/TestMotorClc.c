#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/Motor.h"

#include "MotorFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Motor/MotorInitialise.c")
TEST_FILE("Platform/Motor/MotorEnableDisable.c")
TEST_FILE("Platform/Motor/MotorOnOff.c")

void onBeforeTest(void)
{
	motorFixtureSetUp();
	motorInitialise();
	PMD5bits.CLC2MD = 0;
}

void onAfterTest(void)
{
	motorFixtureTearDown();
}

void test_voltageRegulatorEnabled_onPublished_expectClcIsEnabled(void)
{
	CLC2CON = anyByteWithMaskClear(_CLC2CON_LC2EN_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_TRUE(CLC2CONbits.LC2EN);
}

void test_voltageRegulatorEnabled_onPublished_expectClcInterruptFlagDoesNotGetSetOnPositiveEdge(void)
{
	CLC2CON = anyByteWithMaskSet(_CLC2CON_LC2INTP_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_FALSE(CLC2CONbits.LC2INTP);
}

void test_voltageRegulatorEnabled_onPublished_expectClcInterruptFlagDoesNotGetSetOnNegativeEdge(void)
{
	CLC2CON = anyByteWithMaskSet(_CLC2CON_LC2INTN_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_FALSE(CLC2CONbits.LC2INTN);
}

void test_voltageRegulatorEnabled_onPublished_expectClcCellIs4InputAnd(void)
{
	CLC2CON = anyByteWithMaskSet(0b111 << _CLC2CON_LC2MODE0_POSITION);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0b010, CLC2CONbits.LC2MODE);
}

void test_voltageRegulatorEnabled_onPublished_expectClcOutputIsInvertedBecauseCwgShutdownIsActiveLow(void)
{
	CLC2POL = anyByteWithMaskClear(_CLC2POL_LC2POL_MASK);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_TRUE(CLC2POLbits.LC2POL);
}

void test_voltageRegulatorEnabled_onPublished_expectClcGate1IsConfiguredAsTwoInputOrGate(void)
{
	CLC2POL = anyByte();
	CLC2GLS0 = anyByteExcept(0x0a);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x0a, CLC2GLS0, "CLC2GLS0");
	TEST_ASSERT_FALSE_MESSAGE(CLC2POLbits.LC2G1POL, "LC2G1POL");
}

void test_voltageRegulatorEnabled_onPublished_expectClcGate2IsConfiguredAsLogic1(void)
{
	CLC2POL = anyByte();
	CLC2GLS1 = anyByteExcept(0x00);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x00, CLC2GLS1, "CLC2GLS1");
	TEST_ASSERT_TRUE_MESSAGE(CLC2POLbits.LC2G2POL, "LC2G2POL");
}

void test_voltageRegulatorEnabled_onPublished_expectClcGate3IsConfiguredAsLogic1(void)
{
	CLC2POL = anyByte();
	CLC2GLS2 = anyByteExcept(0x00);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x00, CLC2GLS2, "CLC2GLS2");
	TEST_ASSERT_TRUE_MESSAGE(CLC2POLbits.LC2G3POL, "LC2G3POL");
}

void test_voltageRegulatorEnabled_onPublished_expectClcGate4IsConfiguredAsLogic1(void)
{
	CLC2POL = anyByte();
	CLC2GLS3 = anyByteExcept(0x00);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x00, CLC2GLS3, "CLC2GLS3");
	TEST_ASSERT_TRUE_MESSAGE(CLC2POLbits.LC2G4POL, "LC2G4POL");
}

void test_voltageRegulatorEnabled_onPublished_expectFirstInputChannelIsCurrentSenseComparatorOutput(void)
{
	CLC2SEL0 = anyByteExcept(0b010110);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0b010110, CLC2SEL0);
}

void test_voltageRegulatorEnabled_onPublished_expectSecondInputChannelIsEncoderCcpOutput(void)
{
	CLC2SEL1 = anyByteExcept(0b001111);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0b001111, CLC2SEL1);
}

void test_voltageRegulatorEnabled_onPublished_expectBothInputsAreRoutedToGate1(void)
{
	CLC2GLS0 = anyByteExcept(0b001010);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0b001010, CLC2GLS0);
}

void test_voltageRegulatorEnabled_onPublished_expectGate2HasNoInputs(void)
{
	CLC2GLS1 = anyByteExcept(0);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, CLC2GLS1);
}

void test_voltageRegulatorEnabled_onPublished_expectGate3HasNoInputs(void)
{
	CLC2GLS2 = anyByteExcept(0);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, CLC2GLS2);
}

void test_voltageRegulatorEnabled_onPublished_expectGate4HasNoInputs(void)
{
	CLC2GLS3 = anyByteExcept(0);
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, CLC2GLS3);
}
