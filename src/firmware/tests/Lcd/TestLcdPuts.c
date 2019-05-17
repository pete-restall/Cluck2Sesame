#include <xc.h>
#include <unity.h>

#include "Lcd.h"

#include "FakeLcd.h"
#include "LcdFixture.h"
#include "NonDeterminism.h"

TEST_FILE("Poll.c")
TEST_FILE("Event.c")
TEST_FILE("NearScheduler.c")
TEST_FILE("PowerManagement.c")
TEST_FILE("PwmTimer.c")
TEST_FILE("Clock/ClockInitialise.c")
TEST_FILE("Lcd/LcdInitialise.c")
TEST_FILE("Lcd/LcdEnableDisable.c")
TEST_FILE("Lcd/LcdConfigure.c")
TEST_FILE("Lcd/LcdWrite.c")
TEST_FILE("Lcd/LcdPuts.c")

extern void poll(void);

static void sendLineToLcdAndWaitUntilDone(void);
static void sendOffsetLineToLcdAndWaitUntilDone(uint8_t offset);
static void onLineDisplayed(void *const state);

static uint8_t screen[sizeof(fakeLcdDram) + 1];

void setUp(void)
{
	lcdFixtureInitialise();
	for (uint8_t i = 0; i < sizeof(screen); i++)
		screen[i] = anyByteExcept('\0');

	screen[sizeof(screen) - 1] = '\0';
}

void tearDown(void)
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

static void onLineDisplayed(void *const state)
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
