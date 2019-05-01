#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "NearScheduler.h"

#include "NonDeterminism.h"
#include "NearSchedulerFixture.h"

TEST_FILE("NearScheduler.c")
TEST_FILE("NearSchedulerFixture.c")

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

// TODO: TEST THAT CHANGING TICKS BEFORE EVENT PUBLISHED DOES NOT ALTER ORIGINAL DISPATCH TIME
// TODO: TEST THAT NCO1IF IS CLEARED AFTER EACH TICK WITHOUT PUBLISH
// TODO: TEST THAT NCO1IF IS CLEARED AFTER EACH TICK WITH PUBLISH
// TODO: TEST MORE THAN MAX PENDING DOES NOT OVERWRITE PREVIOUS
// TODO: TEST MORE THAN MAX ADDS DONE BUT SOME DISPATCHED - SHOULD ALL BE DISPATCHED (IE. NO MEMORY ERRORS, NO OVERWRITTEN ENTRIES, ETC.)
