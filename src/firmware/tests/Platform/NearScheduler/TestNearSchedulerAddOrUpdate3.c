#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/NearScheduler.h"

#include "NearSchedulerFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/NearScheduler.c")
TEST_FILE("NearSchedulerFixture.c")

void test_nearSchedulerAddOrUpdate_calledWhenUpdatingPendingSchedule_expectNcoAccumulatorIsNotCleared(void)
{
	nearSchedulerAddOrUpdate(&dummySchedule);

	NCO1ACCU = anyByte();
	uint8_t originalNco1accu = NCO1ACCU;

	NCO1ACCH = anyByte();
	uint8_t originalNco1acch = NCO1ACCH;

	NCO1ACCL = anyByte();
	uint8_t originalNco1accl = NCO1ACCL;

	nearSchedulerAddOrUpdate(&dummySchedule);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalNco1accu, NCO1ACCU, "NCO1ACCU");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalNco1acch, NCO1ACCH, "NCO1ACCH");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(originalNco1accl, NCO1ACCL, "NCO1ACCL");
}

void test_nearSchedulerAddOrUpdate_calledWhenUpdatingPendingScheduleWithZeroTicks_expectNextTickCallsHandler(void)
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

void test_nearSchedulerAddOrUpdate_calledWhenUpdatingPendingScheduleWithZeroTicks_expectOriginalHandlerIsNotCalled(void)
{
	struct NearSchedule pendingSchedule =
	{
		.ticks = 1,
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
	tick();
	tick();
	assertHandlerCalledOnceWith(schedule.state);
}

void test_nearSchedulerAddOrUpdate_calledWhenUpdatingPendingScheduleAndExactNumberOfTicksElapsed_expectHandlerIsNotCalled(void)
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
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&schedule);
	tick();
	assertNoHandlersCalled();
}

void test_nearSchedulerAddOrUpdate_calledWhenUpdatingPendingSchedulesAndRequestedNumberOfTicksPlusOneElapsed_expectHandlerIsCalled(void)
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
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&schedule);
	tick();
	tick();
	assertHandlerCalledOnceWith(schedule.state);
}

void test_nearSchedulerAddOrUpdate_calledWhenUpdatingPendingScheduleAnd255Ticks_expectHandlerIsNotCalledAfter255Ticks(void)
{
	struct NearSchedule pendingSchedule = { .ticks = 0, .handler = &spyHandler };
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

void test_nearSchedulerAddOrUpdate_calledWhenUpdatingPendingScheduleAndRequestedNumberOfTicksIs255_expectHandlerIsCalledAfter256Ticks(void)
{
	struct NearSchedule pendingSchedule =
	{
		.ticks = 0,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAddOrUpdate(&pendingSchedule);

	struct NearSchedule schedule =
	{
		.ticks = 255,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWordExcept((uint16_t) pendingSchedule.state))
	};

	nearSchedulerAddOrUpdate(&schedule);
	for (uint16_t i = 0; i < 256; i++)
		tick();

	assertHandlerCalledOnceWith(schedule.state);
}

void test_nearSchedulerAddOrUpdate_calledWhenUpdatingMultipleSchedulesAtSameTick_expectHandlerIsOnlyCalledForOneOfThem(void)
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
		.handler = &spyHandler,
		.state = (void *) ((int) anyWordExcept((uint16_t) firstSchedule.state))
	};

	nearSchedulerAddOrUpdate(&firstSchedule);
	nearSchedulerAddOrUpdate(&secondSchedule);
	tick();

	assertHandlerCalledOnceWith(secondSchedule.state);
}
