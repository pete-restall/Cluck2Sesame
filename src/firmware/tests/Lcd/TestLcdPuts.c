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

static void sendScreenToLcdAndWaitUntilDone(void);
static void onScreenDisplayed(void *const state);

static uint8_t screen[sizeof(fakeLcdDram)];

void setUp(void)
{
	lcdFixtureInitialise();
	for (uint8_t i = 0; i < sizeof(screen); i++)
		screen[i] = '\0';
}

void tearDown(void)
{
	lcdFixtureShutdown();
}

void test_lcdPuts_calledWithOneCharacterAfterLcdEnabled_expectCorrectBuffer(void)
{
	enableLcdAndWaitUntilDone();

	screen[0] = anyByteExcept('\0');
	screen[1] = '\0';
	sendScreenToLcdAndWaitUntilDone();

	TEST_ASSERT_EQUAL_UINT8(screen[0], fakeLcdDram[0]);
	for (uint8_t i = 1; i < sizeof(fakeLcdDram); i++)
	{
		TEST_ASSERT_EQUAL_UINT8(' ', fakeLcdDram[i]);
	}
}

static void sendScreenToLcdAndWaitUntilDone(void)
{
	static uint8_t lcdCallbackDone;

	static const struct LcdPutsTransaction transaction =
	{
		.buffer = screen,
		.callback = &onScreenDisplayed,
		.state = (void *) &lcdCallbackDone
	};

	lcdCallbackDone = 0;
	lcdPuts(&transaction);
	while (!lcdCallbackDone)
		poll();
}

static void onScreenDisplayed(void *const state)
{
	TEST_ASSERT_NOT_NULL(state);
	*((uint8_t *) state) = 1;
}
