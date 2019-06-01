#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Mock_Event.h"
#include "Platform/VoltageRegulator.h"

#include "VoltageRegulatorFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

static void ensureScheduleHandlerIsNotOmittedByTheCompiler(void);

const struct NearSchedule *requestedSchedule;

void onBeforeTest(void)
{
	requestedSchedule = (const struct NearSchedule *) 0;
}

void onAfterTest(void)
{
	ensureScheduleHandlerIsNotOmittedByTheCompiler();
}

static void ensureScheduleHandlerIsNotOmittedByTheCompiler(void)
{
	static volatile uint8_t neverTrue = 0;
	if (neverTrue)
		requestedSchedule->handler((void *) 0);
}

void assertEventPublishNotCalled(
	EventType type,
	const void *const args,
	int numCalls)
{
	TEST_FAIL_MESSAGE("Expected eventPublish() not to be called");
}

void assertScheduleAddedWithHandler(void)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(requestedSchedule, "Schedule");
	TEST_ASSERT_NOT_NULL_MESSAGE(requestedSchedule->handler, "Handler");
}

void nearSchedulerAdd(const struct NearSchedule *const schedule)
{
	requestedSchedule = schedule;
}

void fullyEnableVoltageRegulator(void)
{
	voltageRegulatorEnable();
	callScheduleHandlerAndForget();
	callScheduleHandlerAndForget();
}

void callScheduleHandlerAndForget(void)
{
	assertScheduleAddedWithHandler();
	callScheduleHandlerIfPresentAndForget();
}

void callScheduleHandlerIfPresentAndForget(void)
{
	if (!requestedSchedule || !requestedSchedule->handler)
		return;

	void *state = requestedSchedule->state;
	NearScheduleHandler handler = requestedSchedule->handler;
	requestedSchedule = (const struct NearSchedule *) 0;
	handler(state);
}

void fullyEnableVoltageRegulatorWithoutAssertions(void)
{
	voltageRegulatorEnable();
	callScheduleHandlerIfPresentAndForget();
	callScheduleHandlerIfPresentAndForget();
}
