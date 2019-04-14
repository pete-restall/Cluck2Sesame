#include <xc.h>
#include <unity.h>

#include "Main.h"
#include "Mock_Clock.h"
#include "Mock_PowerManagement.h"
#include "Mock_Event.h"

TEST_FILE("Initialise.c")

void setUp(void)
{
}

void tearDown(void)
{
}

void test_initialise_called_expectEventSystemIsInitialised(void)
{
	eventInitialise_Expect();
	initialise();
}

void test_initialise_called_expectClockIsInitialisedAfterEvents(void)
{
	eventInitialise_Expect();
	clockInitialise_Expect();
	initialise();
}

void test_initialise_called_expectPowerManagementIsInitialisedAfterClock(void)
{
	clockInitialise_Expect();
	powerManagementInitialise_Expect();
	initialise();
}

void test_initialise_called_expectSystemInitialisedEventIsPublishedLast(void)
{
	static const struct SystemInitialised emptyArgs = { };
	powerManagementInitialise_Expect();
	eventPublish_Expect(SYSTEM_INITIALISED, &emptyArgs);
	initialise();
}
