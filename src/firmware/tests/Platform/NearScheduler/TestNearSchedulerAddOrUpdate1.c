#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/NearScheduler.h"

#include "NearSchedulerFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/NearScheduler.c")
TEST_FILE("NearSchedulerFixture.c")

void test_nearSchedulerAddOrUpdate_calledWhenAddingNewAndNoPendingSchedules_expectNcoIsEnabled(void)
{
	NCO1CON = anyByteWithMaskClear(_NCO1CON_N1EN_MASK);
	uint8_t originalNco1con = NCO1CON;
	nearSchedulerAddOrUpdate(&dummySchedule);
	TEST_ASSERT_TRUE(NCO1CONbits.N1EN);
}

void test_nearSchedulerAddOrUpdate_calledWhenAddingNewAndNoPendingSchedules_expectNcoAccumulatorIsCleared(void)
{
	NCO1ACCU = anyByte();
	NCO1ACCH = anyByte();
	NCO1ACCL = anyByte();
	nearSchedulerAddOrUpdate(&dummySchedule);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, NCO1ACCU, "NCO1ACCU");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, NCO1ACCH, "NCO1ACCH");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, NCO1ACCL, "NCO1ACCL");
}

void test_nearSchedulerAddOrUpdate_calledWhenAddingNewAndPendingSchedules_expectNcoAccumulatorIsNotCleared(void)
{
	nearSchedulerAddOrUpdate(&dummySchedule);

	NCO1ACCU = anyByte();
	uint8_t originalNco1accu = NCO1ACCU;

	NCO1ACCH = anyByte();
	uint8_t originalNco1acch = NCO1ACCH;

	NCO1ACCL = anyByte();
	uint8_t originalNco1accl = NCO1ACCL;

	nearSchedulerAddOrUpdate(&dummySchedule2);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalNco1accu, NCO1ACCU, "NCO1ACCU");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalNco1acch, NCO1ACCH, "NCO1ACCH");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalNco1accl, NCO1ACCL, "NCO1ACCL");
}

void test_nearSchedulerAddOrUpdate_calledWhenAddingNewAndNoPendingSchedulesAndInsufficientTicksElapsed_expectNoHandlersCalled(void)
{
	struct NearSchedule schedule =
	{
		.ticks = anyByteExcept(0),
		.handler = &spyHandler
	};

	nearSchedulerAddOrUpdate(&schedule);
	for (uint8_t i = 0; i < schedule.ticks - 1; i++)
		tick();

	assertNoHandlersCalled();
}

void test_nearSchedulerAddOrUpdate_notCalledButNcoHasTicked_expectNcoInterruptFlagIsCleared(void)
{
	PIR7 = anyByteWithMaskClear(_PIR7_NCO1IF_MASK);
	uint8_t originalPir7 = PIR7;
	tick();
	TEST_ASSERT_EQUAL_UINT8(originalPir7, PIR7);
}

void test_nearSchedulerAddOrUpdate_calledAndWokenFromSleepBecauseOfNonTickEvent_expectNoHandlersAreCalled(void)
{
	const struct NearSchedule schedule =
	{
		.ticks = 1,
		.handler = &spyHandler
	};

	nearSchedulerAddOrUpdate(&schedule);

	PIR7 = anyByteWithMaskClear(_PIR7_NCO1IF_MASK);
	wokenFromSleep();
	assertNoHandlersCalled();
}

void test_nearSchedulerAddOrUpdate_calledWhenHandlerIsNull_expectNoHandlersAreCalled(void)
{
	struct NearSchedule schedule =
	{
		.ticks = 1,
		.handler = (NearScheduleHandler) 0,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&schedule);
	tick();
	assertNoHandlersCalled();
}

void test_nearSchedulerAddOrUpdate_calledWhenAddingNewAndNoPendingSchedulesAndExactNumberOfTicksElapsed_expectHandlerIsCalled(void)
{
	struct NearSchedule schedule =
	{
		.ticks = anyByteExcept(0),
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&schedule);
	for (uint8_t i = 0; i < schedule.ticks; i++)
		tick();

	assertHandlerCalledOnceWith(schedule.state);
}

void test_nearSchedulerAddOrUpdate_calledWhenAddingNewAndWithZeroTicksWhenNoPendingSchedules_expectNextTickCallsHandler(void)
{
	struct NearSchedule schedule =
	{
		.ticks = 0,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&schedule);
	tick();
	assertHandlerCalledOnceWith(schedule.state);
}

void test_nearSchedulerAddOrUpdate_calledWhenAddingNewAndWithZeroTicksWhenPendingSchedules_expectNextTickCallsHandler(void)
{
	struct NearSchedule pendingSchedule =
	{
		.ticks = 7,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&pendingSchedule);

	struct NearSchedule schedule =
	{
		.ticks = 0,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&schedule);
	tick();
	assertHandlerCalledOnceWith(schedule.state);
}

void test_nearSchedulerAddOrUpdate_calledWhenAddingNewAndPendingSchedulesAndExactNumberOfTicksElapsed_expectHandlerIsNotCalled(void)
{
	struct NearSchedule pendingSchedule =
	{
		.ticks = 7,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&pendingSchedule);

	struct NearSchedule schedule =
	{
		.ticks = 1,
		.handler = &spyHandler2,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&schedule);
	tick();
	assertNoHandlersCalled();
}

void test_nearSchedulerAddOrUpdate_calledWhenAddingNewAndPendingSchedulesAndRequestedNumberOfTicksPlusOneElapsed_expectHandlerIsCalled(void)
{
	struct NearSchedule pendingSchedule =
	{
		.ticks = 7,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&pendingSchedule);

	struct NearSchedule schedule =
	{
		.ticks = 1,
		.handler = &spyHandler2,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&schedule);
	tick();
	tick();
	assertHandlerCalledOnceWith(schedule.state);
}

void test_nearSchedulerAddOrUpdate_calledWhenAddingNewAndPendingSchedulesAnd255Ticks_expectHandlerIsNotCalledAfter255Ticks(void)
{
	struct NearSchedule pendingSchedule = { .handler = &dummyHandler };
	nearSchedulerAddOrUpdate(&pendingSchedule);

	struct NearSchedule schedule =
	{
		.ticks = 255,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&schedule);
	for (uint8_t i = 0; i < 255; i++)
		tick();

	assertNoHandlersCalled();
}

void test_nearSchedulerAddOrUpdate_calledWhenAddingNewAndPendingSchedulesAndRequestedNumberOfTicksIs255_expectHandlerIsCalledAfter256Ticks(void)
{
	struct NearSchedule pendingSchedule = { .handler = &dummyHandler };
	nearSchedulerAddOrUpdate(&pendingSchedule);

	struct NearSchedule schedule =
	{
		.ticks = 255,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&schedule);
	for (uint16_t i = 0; i < 256; i++)
		tick();

	assertHandlerCalledOnceWith(schedule.state);
}

void test_nearSchedulerAddOrUpdate_calledWhenAddingNewAndMultipleSchedulesAtSameTick_expectHandlerIsCalledForEachOfThem(void)
{
	struct NearSchedule pendingSchedule = { .handler = &dummyHandler };
	nearSchedulerAddOrUpdate(&pendingSchedule);

	struct NearSchedule firstSchedule =
	{
		.ticks = 0,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	struct NearSchedule secondSchedule =
	{
		.ticks = 0,
		.handler = &spyHandler2,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&firstSchedule);
	nearSchedulerAddOrUpdate(&secondSchedule);
	tick();

	assertHandlerCalledTimes(2);
	assertHandlerCalledWith(firstSchedule.state);
	assertHandlerCalledWith(secondSchedule.state);
}

void test_nearSchedulerAddOrUpdate_calledWhenAddingNewAndMultipleSchedulesAtDifferentTick_expectHandlerIsCalledForEachOfThemInTurn(void)
{
	struct NearSchedule pendingSchedule = { .handler = &dummyHandler };
	nearSchedulerAddOrUpdate(&pendingSchedule);

	struct NearSchedule firstSchedule =
	{
		.ticks = 0,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	struct NearSchedule secondSchedule =
	{
		.ticks = 1,
		.handler = &spyHandler2,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&firstSchedule);
	nearSchedulerAddOrUpdate(&secondSchedule);
	tick();
	assertHandlerCalledTimes(1);
	assertHandlerCalledWith(firstSchedule.state);

	tick();
	assertHandlerCalledTimes(2);
	assertHandlerCalledWith(secondSchedule.state);
}
