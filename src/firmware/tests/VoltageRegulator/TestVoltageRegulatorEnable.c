#include <xc.h>
#include <unity.h>

#include "Mock_Event.h"
#include "VoltageRegulator.h"

#include "TestVoltageRegulatorEnable.h"
#include "NonDeterminism.h"

TEST_FILE("VoltageRegulator.c")

static void ensureScheduleHandlerIsNotOmittedByTheCompiler(void);
static void assertEventPublishNotCalled(
	EventType type,
	const void *const args,
	int numCalls);

static void assertScheduleAddedWithHandler(void);

static const struct NearSchedule *requestedSchedule;

void setUp(void)
{
	requestedSchedule = (const struct NearSchedule *) 0;
}

void tearDown(void)
{
	ensureScheduleHandlerIsNotOmittedByTheCompiler();
}

static void ensureScheduleHandlerIsNotOmittedByTheCompiler(void)
{
	static volatile uint8_t neverTrue = 0;
	if (neverTrue)
		requestedSchedule->handler((void *) 0);
}

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

static void assertEventPublishNotCalled(
	EventType type,
	const void *const args,
	int numCalls)
{
	TEST_FAIL_MESSAGE("Expected eventPublish() not to be called");
}

void test_voltageRegulatorEnable_calledOnce_expectScheduleForRegulatedRailStabilisationTime(void)
{
	voltageRegulatorInitialise();
	voltageRegulatorEnable();
	assertScheduleAddedWithHandler();
	TEST_ASSERT_EQUAL_UINT8(MS_TO_TICKS(64), requestedSchedule->ticks);
}

static void assertScheduleAddedWithHandler(void)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(requestedSchedule, "Schedule");
	TEST_ASSERT_NOT_NULL_MESSAGE(requestedSchedule->handler, "Handler");
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

// TODO: TEST FOLLOWING SCENARIOS:
//
//     voltageRegulatorEnable();
//     regulator startup time elapsed
//     vdd stabilisation time elapsed
//     EXPECT VOLTAGE_REGULATOR_ENABLED EVENT PUBLISHED
//
//     voltageRegulatorEnable();
//     voltageRegulatorDisable();
//     regulator startup time elapsed
//     EXPECT VMCU_SEL (RB0) IS LOW
//
//     voltageRegulatorEnable();
//     voltageRegulatorEnable();
//     voltageRegulatorDisable();
//     regulator startup time elapsed
//     EXPECT VMCU_SEL (RB0) IS HIGH
//
//     voltageRegulatorEnable();
//     regulator startup time elapsed
//     voltageRegulatorDisable();
//     EXPECT VMCU_SEL (RB0) IS LOW
//
//     voltageRegulatorEnable();
//     regulator startup time elapsed
//     voltageRegulatorEnable();
//     voltageRegulatorDisable();
//     EXPECT VMCU_SEL (RB0) IS HIGH
//
//     voltageRegulatorEnable();
//     regulator startup time elapsed
//     voltageRegulatorDisable();
//     EXPECT NO VOLTAGE_REGULATOR_ENABLED EVENT PUBLISHED !
//
//     voltageRegulatorEnable();
//     voltageRegulatorEnable();
//     EXPECT ONLY SINGLE VOLTAGE_REGULATOR_ENABLED EVENT PUBLISHED

void nearSchedulerAdd(const struct NearSchedule *const schedule)
{
	requestedSchedule = schedule;
}
