#include <unity.h>

#include "Main.h"
#include "Mock_Clock.h"

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
