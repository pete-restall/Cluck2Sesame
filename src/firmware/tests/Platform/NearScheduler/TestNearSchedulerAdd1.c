#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/NearScheduler.h"

#include "NearSchedulerFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/NearScheduler.c")
TEST_FILE("NearSchedulerFixture.c")

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
	struct NearSchedule schedule =
	{
		.ticks = anyByteExcept(0),
		.handler = &spyHandler
	};

	nearSchedulerAdd(&schedule);
	for (uint8_t i = 0; i < schedule.ticks - 1; i++)
		tick();

	assertNoHandlersCalled();
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
	const struct NearSchedule schedule =
	{
		.ticks = 1,
		.handler = &spyHandler
	};

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

	assertHandlerCalledOnceWith(schedule.state);
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
	assertHandlerCalledOnceWith(schedule.state);
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
	assertHandlerCalledOnceWith(schedule.state);
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
	assertHandlerCalledOnceWith(schedule.state);
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

	assertHandlerCalledOnceWith(schedule.state);
}

void test_nearSchedulerAdd_calledWhenMultipleSchedulesAtSameTick_expectHandlerIsCalledForEachOfThem(void)
{
	struct NearSchedule pendingSchedule = { .handler = &dummyHandler };
	nearSchedulerAdd(&pendingSchedule);

	struct NearSchedule firstSchedule =
	{
		.ticks = 0,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	struct NearSchedule secondSchedule =
	{
		.ticks = 0,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAdd(&firstSchedule);
	nearSchedulerAdd(&secondSchedule);
	tick();

	assertHandlerCalledTimes(2);
	assertHandlerCalledWith(firstSchedule.state);
	assertHandlerCalledWith(secondSchedule.state);
}

void test_nearSchedulerAdd_calledWhenMultipleSchedulesAtDifferentTick_expectHandlerIsCalledForEachOfThemInTurn(void)
{
	struct NearSchedule pendingSchedule = { .handler = &dummyHandler };
	nearSchedulerAdd(&pendingSchedule);

	struct NearSchedule firstSchedule =
	{
		.ticks = 0,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	struct NearSchedule secondSchedule =
	{
		.ticks = 1,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAdd(&firstSchedule);
	nearSchedulerAdd(&secondSchedule);
	tick();
	assertHandlerCalledTimes(1);
	assertHandlerCalledWith(firstSchedule.state);

	tick();
	assertHandlerCalledTimes(2);
	assertHandlerCalledWith(secondSchedule.state);
}
