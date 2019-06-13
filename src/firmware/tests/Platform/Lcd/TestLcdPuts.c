#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Lcd.h"

#include "FakeLcd.h"
#include "LcdFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Poll.c")
TEST_FILE("Platform/Event.c")
TEST_FILE("Platform/NearScheduler.c")
TEST_FILE("Platform/PowerManagement.c")
TEST_FILE("Platform/PwmTimer.c")
TEST_FILE("Platform/Clock/ClockInitialise.c")
TEST_FILE("Platform/Clock/ClockGetSetNow.c")
TEST_FILE("Platform/Lcd/LcdInitialise.c")
TEST_FILE("Platform/Lcd/LcdEnableDisable.c")
TEST_FILE("Platform/Lcd/LcdConfigure.c")
TEST_FILE("Platform/Lcd/LcdTransaction.c")
TEST_FILE("Platform/Lcd/LcdWrite.c")
TEST_FILE("Platform/Lcd/LcdPuts.c")

extern void poll(void);

static void sendLineToLcdAndWaitUntilDone(void);
static void sendOffsetLineToLcdAndWaitUntilDone(uint8_t offset);
static void onLineDisplayed(void *state);

static uint8_t screen[sizeof(fakeLcdDram) + 1];

static const struct LcdPutsTransaction dummyTransaction = { .buffer = "\0" };

void onBeforeTest(void)
{
	lcdFixtureInitialise();
	for (uint8_t i = 0; i < sizeof(screen); i++)
		screen[i] = anyByteExcept('\0');

	screen[sizeof(screen) - 1] = '\0';
}

void onAfterTest(void)
{
	lcdFixtureShutdown();
}

void test_lcdPuts_calledWithOneCharacterAfterLcdEnabled_expectCorrectDramBuffer(void)
{
	enableLcdAndWaitUntilDone();

	screen[1] = '\0';
	sendLineToLcdAndWaitUntilDone();

	TEST_ASSERT_EQUAL_UINT8(screen[0], fakeLcdDram[0]);
	for (uint8_t i = 1; i < sizeof(fakeLcdDram); i++)
	{
		TEST_ASSERT_EQUAL_UINT8(' ', fakeLcdDram[i]);
	}
}

static void sendLineToLcdAndWaitUntilDone(void)
{
	sendOffsetLineToLcdAndWaitUntilDone(0);
}

static void sendOffsetLineToLcdAndWaitUntilDone(uint8_t offset)
{
	static uint8_t lcdCallbackDone;

	struct LcdPutsTransaction transaction =
	{
		.buffer = &screen[offset],
		.callback = &onLineDisplayed,
		.state = (void *) &lcdCallbackDone
	};

	lcdCallbackDone = 0;
	lcdPuts(&transaction);
	while (!lcdCallbackDone)
		poll();
}

static void onLineDisplayed(void *state)
{
	TEST_ASSERT_NOT_NULL(state);
	*((uint8_t *) state) = 1;
}

void test_lcdPuts_calledWithTwoCharacterAfterLcdEnabled_expectCorrectDramBuffer(void)
{
	enableLcdAndWaitUntilDone();

	screen[2] = '\0';
	sendLineToLcdAndWaitUntilDone();

	TEST_ASSERT_EQUAL_UINT8(screen[0], fakeLcdDram[0]);
	TEST_ASSERT_EQUAL_UINT8(screen[1], fakeLcdDram[1]);
	for (uint8_t i = 2; i < sizeof(fakeLcdDram); i++)
	{
		TEST_ASSERT_EQUAL_UINT8(' ', fakeLcdDram[i]);
	}
}

void test_lcdPuts_calledWithEntireLineOfCharactersAfterLcdEnabled_expectCorrectDramBuffer(void)
{
	enableLcdAndWaitUntilDone();

	screen[sizeof(fakeLcdDram) / 2] = '\0';
	sendLineToLcdAndWaitUntilDone();

	for (uint8_t i = 0; i < sizeof(fakeLcdDram) / 2; i++)
	{
		TEST_ASSERT_EQUAL_UINT8(screen[i], fakeLcdDram[i]);
	}

	for (uint8_t i = sizeof(fakeLcdDram) / 2; i < sizeof(fakeLcdDram); i++)
	{
		TEST_ASSERT_EQUAL_UINT8(' ', fakeLcdDram[i]);
	}
}

void test_lcdPuts_calledTwiceAfterLcdEnabled_expectConcatenatedCharactersInDramBuffer(void)
{
	enableLcdAndWaitUntilDone();

	screen[5] = '\0';
	sendOffsetLineToLcdAndWaitUntilDone(0);
	screen[9] = '\0';
	sendOffsetLineToLcdAndWaitUntilDone(6);

	for (uint8_t i = 0; i < 5; i++)
	{
		TEST_ASSERT_EQUAL_UINT8(screen[i], fakeLcdDram[i]);
	}

	for (uint8_t i = 6; i < 9; i++)
	{
		TEST_ASSERT_EQUAL_UINT8(screen[i], fakeLcdDram[i - 1]);
	}

	for (uint8_t i = 8; i < sizeof(fakeLcdDram); i++)
	{
		TEST_ASSERT_EQUAL_UINT8(' ', fakeLcdDram[i]);
	}
}

void test_lcdPuts_calledWithNullTransaction_expectLcdIsNotSentCommand(void)
{
	enableLcdAndWaitUntilDone();
	lcdPuts((const struct LcdPutsTransaction *) 0);
	fakeLcdAssertNotBusy();
}

void test_lcdPuts_calledWithNullTransaction_expectNearSchedulerIsIdle(void)
{
	enableLcdAndWaitUntilDone();
	lcdPuts((const struct LcdPutsTransaction *) 0);
	TEST_ASSERT_FALSE(NCO1CONbits.N1EN);
}

void test_lcdPuts_calledWithNullBuffer_expectLcdIsNotSentCommand(void)
{
	enableLcdAndWaitUntilDone();

	static const struct LcdPutsTransaction withNullBuffer =
	{
		.buffer = (const uint8_t *) 0
	};

	lcdPuts(&withNullBuffer);
	fakeLcdAssertNotBusy();
}

void test_lcdPuts_calledWithNullBuffer_expectNearSchedulerIsIdle(void)
{
	enableLcdAndWaitUntilDone();

	static const struct LcdPutsTransaction withNullBuffer =
	{
		.buffer = (const uint8_t *) 0
	};

	lcdPuts(&withNullBuffer);
	TEST_ASSERT_FALSE(NCO1CONbits.N1EN);
}

void test_lcdPuts_calledWithNullBuffer_expectCallbackIsStillCalled(void)
{
	enableLcdAndWaitUntilDone();

	static uint8_t callbackWasCalled = 0;
	static const struct LcdPutsTransaction withNullBuffer =
	{
		.buffer = (const uint8_t *) 0,
		.callback = &onLineDisplayed,
		.state = &callbackWasCalled
	};

	lcdPuts(&withNullBuffer);
	TEST_ASSERT_TRUE(callbackWasCalled);
}

void test_lcdPuts_calledBeforeLcdEnabled_expectLcdIsNotSentCommand(void)
{
	lcdPuts(&dummyTransaction);
	fakeLcdAssertNotBusy();
}

void test_lcdPuts_calledBeforeLcdEnabled_expectNearSchedulerIsIdle(void)
{
	lcdPuts(&dummyTransaction);
	TEST_ASSERT_FALSE(NCO1CONbits.N1EN);
}

void test_lcdPuts_calledBeforeLcdConfigured_expectLcdIsNotSentCommand(void)
{
	lcdEnable();
	lcdPuts(&dummyTransaction);
	fakeLcdAssertStateIsValid();
}

void test_lcdPuts_calledWhileBusy_expectLcdIsNotSentCommand(void)
{
	lcdEnable();
	lcdPuts(&dummyTransaction);
	lcdPuts(&dummyTransaction);
	fakeLcdAssertStateIsValid();
}

void test_lcdPuts_calledAfterLcdDisabled_expectLcdIsNotSentCommand(void)
{
	enableLcdAndWaitUntilDone();
	lcdDisable();
	lcdPuts(&dummyTransaction);
	fakeLcdAssertNotBusy();
}

void test_lcdPuts_calledAfterLcdDisabled_expectNearSchedulerIsIdle(void)
{
	enableLcdAndWaitUntilDone();
	lcdDisable();
	lcdPuts(&dummyTransaction);
	TEST_ASSERT_FALSE(NCO1CONbits.N1EN);
}
