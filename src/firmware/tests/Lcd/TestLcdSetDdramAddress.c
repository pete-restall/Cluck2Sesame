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
TEST_FILE("Lcd/LcdTransaction.c")
TEST_FILE("Lcd/LcdWrite.c")
TEST_FILE("Lcd/LcdSetDdramAddress.c")

extern void poll(void);

static void sendSetDdramAddressToLcdAndWaitUntilDone(void);
static void onAddressSet(void *const state);

static struct LcdSetAddressTransaction addressTransaction;

void setUp(void)
{
	lcdFixtureInitialise();
}

void tearDown(void)
{
	lcdFixtureShutdown();
}

void test_lcdSetDdramAddress_calledAfterLcdEnabled_expectCorrectDdramAddress(void)
{
	enableLcdAndWaitUntilDone();

	addressTransaction.address = anyByteWithMaskClear(0b10000000);
	sendSetDdramAddressToLcdAndWaitUntilDone();
	fakeLcdAssertDdramAddressRegisterIs(addressTransaction.address);
}

static void sendSetDdramAddressToLcdAndWaitUntilDone(void)
{
	static uint8_t lcdCallbackDone;

	addressTransaction.callback = &onAddressSet;
	addressTransaction.state = (void *) &lcdCallbackDone;

	lcdCallbackDone = 0;
	lcdSetDdramAddress(&addressTransaction);
	while (!lcdCallbackDone)
		poll();
}

static void onAddressSet(void *const state)
{
	TEST_ASSERT_NOT_NULL(state);
	*((uint8_t *) state) = 1;
}

void test_lcdSetDdramAddress_calledWithNullTransaction_expectLcdIsNotSentCommand(void)
{
	enableLcdAndWaitUntilDone();
	lcdSetDdramAddress((const struct LcdSetDdramAddressTransaction *) 0);
	fakeLcdAssertNotBusy();
}

void test_lcdSetDdramAddress_calledWithNullTransaction_expectNearSchedulerIsIdle(void)
{
	enableLcdAndWaitUntilDone();
	lcdSetDdramAddress((const struct LcdSetDdramAddressTransaction *) 0);
	TEST_ASSERT_FALSE(NCO1CONbits.N1EN);
}

void test_lcdSetDdramAddress_calledBeforeLcdEnabled_expectLcdIsNotSentCommand(void)
{
	lcdSetDdramAddress(&addressTransaction);
	fakeLcdAssertNotBusy();
}

void test_lcdSetDdramAddress_calledBeforeLcdEnabled_expectNearSchedulerIsIdle(void)
{
	lcdSetDdramAddress(&addressTransaction);
	TEST_ASSERT_FALSE(NCO1CONbits.N1EN);
}

void test_lcdSetDdramAddress_calledBeforeLcdConfigured_expectLcdIsNotSentCommand(void)
{
	lcdEnable();
	lcdSetDdramAddress(&addressTransaction);
	fakeLcdAssertStateIsValid();
}

void test_lcdSetDdramAddress_calledWhileBusy_expectLcdIsNotSentCommand(void)
{
	lcdEnable();
	lcdSetDdramAddress(&addressTransaction);
	lcdSetDdramAddress(&addressTransaction);
	fakeLcdAssertStateIsValid();
}

void test_lcdSetDdramAddress_calledAfterLcdDisabled_expectLcdIsNotSentCommand(void)
{
	enableLcdAndWaitUntilDone();
	lcdDisable();
	lcdSetDdramAddress(&addressTransaction);
	fakeLcdAssertNotBusy();
}

void test_lcdSetDdramAddress_calledAfterLcdDisabled_expectNearSchedulerIsIdle(void)
{
	enableLcdAndWaitUntilDone();
	lcdDisable();
	lcdSetDdramAddress(&addressTransaction);
	TEST_ASSERT_FALSE(NCO1CONbits.N1EN);
}

void test_lcdSetDdramAddress_calledTwice_expectCorrectDdramAddress(void)
{
	enableLcdAndWaitUntilDone();
	sendSetDdramAddressToLcdAndWaitUntilDone();

	addressTransaction.address = anyByteWithMaskClear(0b10000000);
	sendSetDdramAddressToLcdAndWaitUntilDone();
	fakeLcdAssertDdramAddressRegisterIs(addressTransaction.address);
}
