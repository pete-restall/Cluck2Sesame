#include <xc.h>
#include <unity.h>

#include "Event.h"
#include "Clock.h"
#include "PowerManagement.h"
#include "NearScheduler.h"
#include "PwmTimer.h"
#include "Lcd.h"

#include "LcdFixture.h"
#include "NonDeterminism.h"

TEST_FILE("Poll.c")
TEST_FILE("Clock/ClockInitialise.c")
TEST_FILE("Lcd/LcdInitialise.c")
TEST_FILE("Lcd/LcdEnableDisable.c")
TEST_FILE("Lcd/LcdConfigure.c")

extern void poll(void);

static void voltageRegulatorInitialise(void);
static void onLcdEnabled(const struct Event *event);
void voltageRegulatorDisable(void);

static uint8_t isLcdEnabled;

void setUp(void)
{
	eventInitialise();
	clockInitialise();
	powerManagementInitialise();
	nearSchedulerInitialise();
	pwmTimerInitialise();
	voltageRegulatorInitialise();
	lcdInitialise();
	fakeLcdInitialise();

	isLcdEnabled = 0;
}

static void voltageRegulatorInitialise(void)
{
	ANSELBbits.ANSB2 = 0;
	LATBbits.LATB2 = 0;
	TRISBbits.TRISB2 = 0;
}

void tearDown(void)
{
	fakeLcdShutdown();
	voltageRegulatorDisable();
}

void test_lcdConfigure_called_expectFirstByteSentToLcdIsFunctionSetForByteMode(void)
{
	lcdEnable();
	while (!fakeLcdIsSessionInvalid && fakeLcdData == 0)
		poll();

	TEST_ASSERT_FALSE_MESSAGE(fakeLcdRs, "RS");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0b00110000, fakeLcdData, "DATA");
}

void test_lcdConfigure_called_expectLcdIsConfiguredInNybbleMode(void)
{
	static const struct EventSubscription onLcdEnabledSubscription =
	{
		.type = LCD_ENABLED,
		.handler = &onLcdEnabled,
		.state = (void *) 0
	};

	eventSubscribe(&onLcdEnabledSubscription);

	lcdEnable();
	while (!fakeLcdIsSessionInvalid && !isLcdEnabled)
		poll();

	TEST_ASSERT_TRUE(fakeLcdIsNybbleMode);
}

static void onLcdEnabled(const struct Event *event)
{
	isLcdEnabled = 1;
}

void voltageRegulatorEnable(void)
{
	LATBbits.LATB2 = 1;
}

void voltageRegulatorDisable(void)
{
	LATBbits.LATB2 = 0;
}

uint8_t voltageRegulatorIsEnabled(void)
{
	return LATBbits.LATB2 != 0;
}
