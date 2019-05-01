#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "NearScheduler.h"

#include "NonDeterminism.h"
#include "NearSchedulerFixture.h"

#define MAX_SCHEDULES 8

TEST_FILE("NearScheduler.c")
TEST_FILE("NearSchedulerFixture.c")

static void assertNcoInterruptFlagIsClear(const struct NearSchedule *const schedule);

void test_nearSchedulerAdd_called_expectHandlerIsNotCalledOnTickRollover(void)
{
	struct NearSchedule schedule =
	{
		.ticks = 0,
		.handler = &spyHandler,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAdd(&schedule);
	for (uint16_t i = 0; i < 513; i++)
		tick();

	assertHandlerCalledTimes(1);
}

void test_nearSchedulerAdd_called_expectHandlerIsCalledWithClearedNcoInterruptFlag(void)
{
	struct NearSchedule schedule =
	{
		.ticks = 0,
		.handler = &assertNcoInterruptFlagIsClear,
		.state = (void *) ((int) anyWord())
	};

	nearSchedulerAdd(&schedule);
	tick();
}

static void assertNcoInterruptFlagIsClear(const struct NearSchedule *const schedule)
{
	TEST_ASSERT_FALSE(PIR7bits.NCO1IF);
}

void test_nearSchedulerAdd_calledMoreThanBufferSize_expectExistingHandlersAreNotOverwritten(void)
{
	struct NearSchedule schedules[MAX_SCHEDULES + 1];
	for (uint8_t i = 0; i < MAX_SCHEDULES + 1; i++)
	{
		schedules[i].ticks = 0;
		schedules[i].handler = &spyHandler;
		schedules[i].state = (void *) ((int) i);
		nearSchedulerAdd(&schedules[i]);
	};

	tick();
	assertHandlerCalledTimes(MAX_SCHEDULES);
	for (uint8_t i = 0; i < MAX_SCHEDULES; i++)
		assertHandlerCalledWith(&schedules[i]);
}

// TODO: TEST MORE THAN MAX PENDING DOES NOT OVERWRITE PREVIOUS
// TODO: TEST MORE THAN MAX ADDS DONE BUT SOME DISPATCHED - SHOULD ALL BE DISPATCHED (IE. NO MEMORY ERRORS, NO OVERWRITTEN ENTRIES, ETC.)
// TODO: TEST THAT WHEN NO MORE SLOTS AND THE HANDLER ADDS A NEW HANDLER, IT WORKS (IE. THE HANDLER IS NULL WHEN CALLED; CAN PROBABLY THEN CHANGE THE SIGNATURE OF THE HANDLERS TO 'void handler(void *state)'.
