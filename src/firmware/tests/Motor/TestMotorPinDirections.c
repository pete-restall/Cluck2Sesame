#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Event.h"
#include "Mock_VoltageRegulator.h"
#include "Motor.h"

#include "MotorFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

TEST_FILE("Motor/MotorInitialise.c")

void onBeforeTest(void)
{
	eventInitialise();
	motorInitialise();
}

void onAfterTest(void)
{
}

void test_voltageRegulatorEnabled_onPublished_expectEncoderPinsAreInputs(void)
{
	static const uint8_t encoderPins = _TRISC_TRISC2_MASK | _TRISC_TRISC3_MASK;
	uint8_t originalTrisc = TRISC;
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(originalTrisc | encoderPins, TRISC);
}

void test_voltageRegulatorEnabled_onPublished_expectCurrentSensePinIsInput(void)
{
	uint8_t originalTrisb = TRISB;
	publishVoltageRegulatorEnabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(originalTrisb | _TRISB_TRISB1_MASK, TRISB);
}

void test_voltageRegulatorDisabled_onPublished_expectEncoderPinsAreOutputs(void)
{
	static const uint8_t encoderPins = _TRISC_TRISC2_MASK | _TRISC_TRISC3_MASK;
	TRISC = anyByteWithMaskSet(encoderPins);
	uint8_t originalTrisc = TRISC;
	publishVoltageRegulatorDisabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(originalTrisc & ~encoderPins, TRISC);
}

void test_voltageRegulatorDisabled_onPublished_expectCurrentSensePinIsOutput(void)
{
	TRISB = anyByteWithMaskSet(_TRISB_TRISB1_MASK);
	uint8_t originalTrisb = TRISB;
	publishVoltageRegulatorDisabled();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(originalTrisb & ~_TRISB_TRISB1_MASK, TRISB);
}
