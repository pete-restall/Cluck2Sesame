#include <xc.h>
#include <unity.h>

#include "LcdFixture.h"
#include "NonDeterminism.h"

volatile uint8_t fakeLcdRs;
volatile uint8_t fakeLcdData;
volatile uint8_t fakeLcdInitialFunctionSetCount;
volatile uint8_t fakeLcdIsConfigured;
volatile uint8_t fakeLcdIsNybbleMode;
volatile uint8_t fakeLcdClocked;
volatile uint8_t fakeLcdCommand;
volatile uint8_t fakeLcdBusyFlag;
volatile uint32_t fakeLcdCommandIndex;

volatile uint8_t fakeLcdSessionIndex;
volatile uint8_t fakeLcdIsSessionInvalid;

void fakeLcdInitialise(void)
{
	fakeLcdSessionIndex++;
}

void fakeLcdShutdown(void)
{
	TEST_ASSERT_FALSE_MESSAGE(fakeLcdIsSessionInvalid, "LCD violations !");
}
