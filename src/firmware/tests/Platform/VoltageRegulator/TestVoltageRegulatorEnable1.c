#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Mock_Event.h"
#include "Platform/VoltageRegulator.h"

#include "VoltageRegulatorFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/VoltageRegulator.c")
TEST_FILE("Platform/VoltageRegulatorFixture.c")

void test_voltageRegulatorEnable_calledOnceWhenEnablePinIsLow_expectEnablePinIsHigh(void)
{
	voltageRegulatorInitialise();
	LATB = anyByteWithMaskClear(_LATB_LATB2_MASK);
	voltageRegulatorEnable();
	TEST_ASSERT_TRUE(LATBbits.LATB2);
}

void test_voltageRegulatorEnable_calledOnceWhenEnablePinIsHigh_expectEnablePinIsHigh(void)
{
	voltageRegulatorInitialise();
	LATB = anyByteWithMaskSet(_LATB_LATB2_MASK);
	voltageRegulatorEnable();
	TEST_ASSERT_TRUE(LATBbits.LATB2);
}

void test_voltageRegulatorEnable_calledOnceWhenVmcuSelPinIsLow_expectPinRemainsLow(void)
{
	voltageRegulatorInitialise();
	LATB = anyByteWithMaskClear(_LATB_LATB0_MASK);
	voltageRegulatorEnable();
	TEST_ASSERT_FALSE(LATBbits.LATB0);
}

void test_voltageRegulatorEnable_calledOnceWhenVmcuSelPinIsHigh_expectPinRemainsHigh(void)
{
	voltageRegulatorInitialise();
	LATB = anyByteWithMaskSet(_LATB_LATB0_MASK);
	voltageRegulatorEnable();
	TEST_ASSERT_TRUE(LATBbits.LATB0);
}

void test_voltageRegulatorEnable_calledOnce_expectNonVoltageRegulatorPinsRemainUnchanged(void)
{
	static const uint8_t usedPins = _LATB_LATB0_MASK | _LATB_LATB2_MASK;
	LATB = anyByte();
	uint8_t expectedLatb = LATB | usedPins;
	voltageRegulatorInitialise();

	voltageRegulatorEnable();
	TEST_ASSERT_EQUAL_UINT8(expectedLatb, LATB | usedPins);
}

void test_voltageRegulatorEnable_calledOnce_expectNoEventsPublished(void)
{
	voltageRegulatorInitialise();
	eventPublish_StubWithCallback(assertEventPublishNotCalled);
	voltageRegulatorEnable();
}

void test_voltageRegulatorEnable_calledOnce_expectScheduleForRegulatedRailStabilisationTime(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();
	assertScheduleAddedWithHandler();
	TEST_ASSERT_EQUAL_UINT8(MS_TO_TICKS(64), requestedSchedule->ticks);
}

void test_voltageRegulatorEnable_calledOnce_expectMcuVoltageRailIsSwitchedToRegulatedRail(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();
	assertScheduleAddedWithHandler();

	LATB = anyByteWithMaskClear(_LATB_LATB0_MASK);
	uint8_t originalLatb = LATB;

	requestedSchedule->handler(requestedSchedule->state);
	TEST_ASSERT_EQUAL_UINT8(originalLatb | _LATB_LATB0_MASK, LATB);
}

void test_voltageRegulatorEnable_calledOnce_expectScheduleForMcuVoltageRailStabilisationTime(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();

	void *state = requestedSchedule->state;
	NearScheduleHandler handler = requestedSchedule->handler;
	requestedSchedule = (const struct NearSchedule *) 0;
	handler(state);

	assertScheduleAddedWithHandler();
	TEST_ASSERT_EQUAL_UINT8(MS_TO_TICKS(4), requestedSchedule->ticks);
}
