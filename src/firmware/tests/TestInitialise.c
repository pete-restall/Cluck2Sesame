#include <unity.h>

#include "Main.h"
#include "Mock_Clock.h"
#include "Mock_Event.h"

TEST_FILE("Initialise.c")

void setUp(void)
{
}

void tearDown(void)
{
}

void test_initialise_called_expectClockIsInitialised(void)
{
	clockInitialise_Expect();
	initialise();
}

void test_initialise_called_expectSystemInitialisedEventIsPublishedLast(void)
{
	static const struct SystemInitialised emptyArgs = { };
	clockInitialise_Expect();
	eventPublish_Expect(SYSTEM_INITIALISED, &emptyArgs);
	initialise();
}
