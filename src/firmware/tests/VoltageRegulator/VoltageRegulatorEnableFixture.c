#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Mock_Event.h"
#include "VoltageRegulator.h"

#include "VoltageRegulatorEnableFixture.h"
#include "NonDeterminism.h"

static void ensureScheduleHandlerIsNotOmittedByTheCompiler(void);

const struct NearSchedule *requestedSchedule;

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
