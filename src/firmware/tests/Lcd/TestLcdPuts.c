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
static void onLineDisplayed(void *const state);

static uint8_t oneLine[sizeof(fakeLcdDram) / 2 + 1];

void setUp(void)
{
	lcdFixtureInitialise();
	for (uint8_t i = 0; i < sizeof(oneLine); i++)
		oneLine[i] = anyByte();

	oneLine[sizeof(oneLine) - 1] = '\0';
}

void tearDown(void)
{
	lcdFixtureShutdown();
}

void test_lcdPuts_calledWithOneCharacterAfterLcdEnabled_expectCorrectDramBuffer(void)
{
	enableLcdAndWaitUntilDone();

	oneLine[0] = anyByteExcept('\0');
	oneLine[1] = '\0';
	sendLineToLcdAndWaitUntilDone();

	TEST_ASSERT_EQUAL_UINT8(oneLine[0], fakeLcdDram[0]);
	for (uint8_t i = 1; i < sizeof(fakeLcdDram); i++)
	{
		TEST_ASSERT_EQUAL_UINT8(' ', fakeLcdDram[i]);
	}
}

static void sendLineToLcdAndWaitUntilDone(void)
{
	static uint8_t lcdCallbackDone;

	static const struct LcdPutsTransaction transaction =
	{
		.buffer = oneLine,
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

	oneLine[0] = anyByteExcept('\0');
	oneLine[1] = anyByteExcept('\0');
	oneLine[2] = '\0';
	sendLineToLcdAndWaitUntilDone();

	TEST_ASSERT_EQUAL_UINT8(oneLine[0], fakeLcdDram[0]);
	TEST_ASSERT_EQUAL_UINT8(oneLine[1], fakeLcdDram[1]);
	for (uint8_t i = 2; i < sizeof(fakeLcdDram); i++)
	{
		TEST_ASSERT_EQUAL_UINT8(' ', fakeLcdDram[i]);
	}
}

void test_lcdPuts_calledWithEntireLineOfCharactersAfterLcdEnabled_expectCorrectDramBuffer(void)
{
	enableLcdAndWaitUntilDone();
	sendLineToLcdAndWaitUntilDone();

	for (uint8_t i = 0; i < sizeof(fakeLcdDram) / 2; i++)
	{
		TEST_ASSERT_EQUAL_UINT8(oneLine[i], fakeLcdDram[i]);
	}

	for (uint8_t i = sizeof(fakeLcdDram) / 2; i < sizeof(fakeLcdDram); i++)
	{
		TEST_ASSERT_EQUAL_UINT8(' ', fakeLcdDram[i]);
	}
}
