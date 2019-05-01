#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "TestNearSchedulerAdd.h"
#include "NearScheduler.h"

#include "NonDeterminism.h"

TEST_FILE("NearScheduler.c")

static void tick(void);
static void wokenFromSleep(void);
static void assertNoHandlersCalled(void);
static void assertHandlerCalledOnceWith(const struct NearSchedule *const schedule);
static void spyHandler(const struct NearSchedule *const schedule);
static void dummyHandler(const struct NearSchedule *const schedule) { }

static const struct NearSchedule dummySchedule =
{
	.ticks = 0,
	.handler = &dummyHandler,
	.state = (void *) 0
};

static const struct EventSubscription *onWokenFromSleep;
static struct Event wokenFromSleepEvent;

static uint8_t numberOfHandlerCalls;
static const struct NearSchedule *lastHandlerSchedule;

void setUp(void)
{
	onWokenFromSleep = (struct EventSubscription *) 0;
	nearSchedulerInitialise();
	numberOfHandlerCalls = 0;

	// HACK: Simulator does not respect NCO clock source, so NCO1ACC*
	// manipulation does not produce the correct values if there is an
	// increment.
	NCO1INCU = 0;
	NCO1INCH = 0;
	NCO1INCL = 0;
}

void tearDown(void)
{
}

void test_nearSchedulerAdd_calledWhenNoPendingSchedules_expectNcoIsEnabled(void)
{
	NCO1CON = anyByteWithMaskClear(_NCO1CON_N1EN_MASK);
	uint8_t originalNco1con = NCO1CON;
	nearSchedulerAdd(&dummySchedule);
	TEST_ASSERT_TRUE(NCO1CONbits.N1EN);
}

void test_nearSchedulerAdd_calledWhenNoPendingSchedules_expectNcoAccumulatorIsCleared(void)
{
	NCO1ACCU = anyByte();
	NCO1ACCH = anyByte();
	NCO1ACCL = anyByte();
	nearSchedulerAdd(&dummySchedule);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, NCO1ACCU, "NCO1ACCU");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, NCO1ACCH, "NCO1ACCH");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, NCO1ACCL, "NCO1ACCL");
}

void test_nearSchedulerAdd_calledWhenPendingSchedules_expectNcoAccumulatorIsNotCleared(void)
{
	nearSchedulerAdd(&dummySchedule);

	NCO1ACCU = anyByte();
	uint8_t originalNco1accu = NCO1ACCU;

	NCO1ACCH = anyByte();
	uint8_t originalNco1acch = NCO1ACCH;

	NCO1ACCL = anyByte();
	uint8_t originalNco1accl = NCO1ACCL;

	nearSchedulerAdd(&dummySchedule);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalNco1accu, NCO1ACCU, "NCO1ACCU");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalNco1acch, NCO1ACCH, "NCO1ACCH");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalNco1accl, NCO1ACCL, "NCO1ACCL");
}

void test_nearSchedulerAdd_calledWhenNoPendingSchedulesAndInsufficientTicksElapsed_expectNoHandlersCalled(void)
{
	struct NearSchedule schedule = { .ticks = anyByteExcept(0) };

	nearSchedulerAdd(&schedule);
	for (uint8_t i = 0; i < schedule.ticks - 1; i++)
		tick();

	assertNoHandlersCalled();
}

static void tick(void)
{
	PIR7bits.NCO1IF = 1;
	wokenFromSleep();
}

static void wokenFromSleep(void)
{
	if (onWokenFromSleep && onWokenFromSleep->handler)
	{
		onWokenFromSleep->handler(&wokenFromSleepEvent);
	}
	else
	{
		TEST_FAIL_MESSAGE("No WokenFromSleep handler");
	}
}

static void assertNoHandlersCalled(void)
{
	TEST_ASSERT_EQUAL_UINT8(0, numberOfHandlerCalls);
}

void test_nearSchedulerAdd_notCalledButNcoHasTicked_expectNcoInterruptFlagIsCleared(void)
{
	PIR7 = anyByteWithMaskClear(_PIR7_NCO1IF_MASK);
	uint8_t originalPir7 = PIR7;
	tick();
	TEST_ASSERT_EQUAL_UINT8(originalPir7, PIR7);
}

void test_nearSchedulerAdd_calledAndWokenFromSleepBecauseOfNonTickEvent_expectNoHandlersAreCalled(void)
{
	struct NearSchedule schedule = { .ticks = 1 };
	nearSchedulerAdd(&schedule);

	PIR7 = anyByteWithMaskClear(_PIR7_NCO1IF_MASK);
	wokenFromSleep();
	assertNoHandlersCalled();
}

void test_nearSchedulerAdd_calledWhenHandlerIsNull_expectNoHandlersAreCalled(void)
{
	struct NearSchedule schedule =
	{
		.ticks = 1,
		.handler = (NearScheduleHandler) 0,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAdd(&schedule);
	tick();
	assertNoHandlersCalled();
}

void test_nearSchedulerAdd_calledWhenNoPendingSchedulesAndExactNumberOfTicksElapsed_expectHandlerIsCalled(void)
{
	struct NearSchedule schedule =
	{
		.ticks = anyByteExcept(0),
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAdd(&schedule);
	for (uint8_t i = 0; i < schedule.ticks; i++)
		tick();

	assertHandlerCalledOnceWith(&schedule);
}

void test_nearSchedulerAdd_calledWithZeroTicksWhenNoPendingSchedules_expectNextTickCallsHandler(void)
{
	struct NearSchedule schedule =
	{
		.ticks = 0,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAdd(&schedule);
	tick();
	assertHandlerCalledOnceWith(&schedule);
}

void test_nearSchedulerAdd_calledWithZeroTicksWhenPendingSchedules_expectNextTickCallsHandler(void)
{
	struct NearSchedule pendingSchedule =
	{
		.ticks = 7,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAdd(&pendingSchedule);

	struct NearSchedule schedule =
	{
		.ticks = 0,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAdd(&schedule);
	tick();
	assertHandlerCalledOnceWith(&schedule);
}

void test_nearSchedulerAdd_calledWhenPendingSchedulesAndExactNumberOfTicksElapsed_expectHandlerIsNotCalled(void)
{
	struct NearSchedule pendingSchedule =
	{
		.ticks = 7,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAdd(&pendingSchedule);

	struct NearSchedule schedule =
	{
		.ticks = 1,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAdd(&schedule);
	tick();
	assertNoHandlersCalled();
}

void test_nearSchedulerAdd_calledWhenPendingSchedulesAndRequestedNumberOfTicksPlusOneElapsed_expectHandlerIsCalled(void)
{
	struct NearSchedule pendingSchedule =
	{
		.ticks = 7,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAdd(&pendingSchedule);

	struct NearSchedule schedule =
	{
		.ticks = 1,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAdd(&schedule);
	tick();
	tick();
	assertHandlerCalledOnceWith(&schedule);
}

void test_nearSchedulerAdd_calledWhenPendingSchedulesAnd255Ticks_expectHandlerIsNotCalledAfter255Ticks(void)
{
	struct NearSchedule pendingSchedule = { .handler = &dummyHandler };
	nearSchedulerAdd(&pendingSchedule);

	struct NearSchedule schedule =
	{
		.ticks = 255,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAdd(&schedule);
	for (uint8_t i = 0; i < 255; i++)
		tick();

	assertNoHandlersCalled();
}

void test_nearSchedulerAdd_calledWhenPendingSchedulesAndRequestedNumberOfTicksIs255_expectHandlerIsCalledAfter256Ticks(void)
{
	struct NearSchedule pendingSchedule = { .handler = &dummyHandler };
	nearSchedulerAdd(&pendingSchedule);

	struct NearSchedule schedule =
	{
		.ticks = 255,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAdd(&schedule);
	for (uint16_t i = 0; i < 256; i++)
		tick();

	assertHandlerCalledOnceWith(&schedule);
}

// TODO: TEST FOR 255 TICKS WHEN PENDING - DISPATCHED AT TICKS + 1 (ROLLOVER)
// TODO: TEST FOR MULTIPLE SCHEDULED EVENTS AT SAME TICK - ALL SHOULD BE CALLED
// TODO: TEST FOR MULTIPLE SCHEDULED EVENTS AT DIFFERENT TICK - ALL SHOULD BE CALLED, AT DIFFERENT TIMES
// TODO: TEST FOR TICKS ROLLOVER - SHOULDN'T EXECUTE THE SCHEDULE HANDLER AGAIN
// TODO: TEST THAT CHANGING TICKS BEFORE EVENT PUBLISHED DOES NOT ALTER ORIGINAL DISPATCH TIME
// TODO: TEST THAT NCO1IF IS CLEARED AFTER EACH TICK WITHOUT PUBLISH
// TODO: TEST THAT NCO1IF IS CLEARED AFTER EACH TICK WITH PUBLISH
// TODO: TEST MORE THAN MAX PENDING DOES NOT OVERWRITE PREVIOUS
// TODO: TEST MORE THAN MAX ADDS DONE BUT SOME DISPATCHED - SHOULD ALL BE DISPATCHED (IE. NO MEMORY ERRORS, NO OVERWRITTEN ENTRIES, ETC.)

void eventSubscribe(const struct EventSubscription *const subscription)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription, "Subscription");
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription->handler, "Handler");
	TEST_ASSERT_EQUAL_MESSAGE(
		WOKEN_FROM_SLEEP,
		subscription->type,
		"Unexpected type");

	static const struct WokenFromSleep emptyEventArgs = { };
	onWokenFromSleep = subscription;
	wokenFromSleepEvent.type = subscription->type;
	wokenFromSleepEvent.state = subscription->state;
	wokenFromSleepEvent.args = &emptyEventArgs;
}

static void spyHandler(const struct NearSchedule *const schedule)
{
	numberOfHandlerCalls++;
	lastHandlerSchedule = schedule;
}

static void assertHandlerCalledOnceWith(const struct NearSchedule *const schedule)
{
	TEST_ASSERT_EQUAL_MESSAGE(1, numberOfHandlerCalls, "Times");
	TEST_ASSERT_NOT_NULL_MESSAGE(lastHandlerSchedule, "Null schedule");
	TEST_ASSERT_NOT_EQUAL_MESSAGE(schedule, lastHandlerSchedule, "Expected copy");
	TEST_ASSERT_EQUAL_MESSAGE(schedule->handler, lastHandlerSchedule->handler, "Handler");
	TEST_ASSERT_EQUAL_MESSAGE(schedule->state, lastHandlerSchedule->state, "State");
}
