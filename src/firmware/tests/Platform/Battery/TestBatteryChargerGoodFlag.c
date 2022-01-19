#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/Battery.h"

#include "BatteryFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Battery.c")

static void onBatteryChargerEnabled(const struct Event *event);
static void onBatteryChargerDisabled(const struct Event *event);
static void stubAllParametersThatWillEnableCharging(void);

static const struct BatteryChargerEnabled *batteryChargerEnabledEventArgs;
static const struct BatteryChargerDisabled *batteryChargerDisabledEventArgs;

void onBeforeTest(void)
{
	batteryFixtureSetUp();

	static const struct EventSubscription onBatteryChargerEnabledSubscription =
	{
		.type = BATTERY_CHARGER_ENABLED,
		.handler = &onBatteryChargerEnabled,
		.state = (void *) 0
	};

	eventSubscribe(&onBatteryChargerEnabledSubscription);

	static const struct EventSubscription onBatteryChargerDisabledSubscription =
	{
		.type = BATTERY_CHARGER_DISABLED,
		.handler = &onBatteryChargerDisabled,
		.state = (void *) 0
	};

	eventSubscribe(&onBatteryChargerDisabledSubscription);

	batteryChargerEnabledEventArgs = (const struct BatteryChargerEnabled *) 0;
}

static void onBatteryChargerEnabled(const struct Event *event)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(event, "Null event !");
	batteryChargerEnabledEventArgs = (const struct BatteryChargerEnabled *) event->args;
	TEST_ASSERT_NOT_NULL_MESSAGE(batteryChargerEnabledEventArgs, "Null event args !");
}

static void onBatteryChargerDisabled(const struct Event *event)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(event, "Null event !");
	batteryChargerDisabledEventArgs = (const struct BatteryChargerDisabled *) event->args;
	TEST_ASSERT_NOT_NULL_MESSAGE(batteryChargerDisabledEventArgs, "Null event args !");
}

void onAfterTest(void)
{
	batteryFixtureTearDown();
}

void test_batteryInitialise_expectNoBatteryChargerEnabledEventIsPublished(void)
{
	batteryInitialise();
	dispatchAllEvents();
	TEST_ASSERT_NULL(batteryChargerEnabledEventArgs);
}

void test_batteryInitialise_expectBatteryChargerDisabledEventIsNotPublished(void)
{
	batteryInitialise();
	dispatchAllEvents();
	TEST_ASSERT_NULL(batteryChargerDisabledEventArgs);
}

void test_batteryChargerEnabled_eventWhenChargerGoodPinIsHighWhenOtherParametersAllowCharging_expectNoBatteryChargerEnabledEventIsPublished(void)
{
	batteryInitialise();
	stubAllParametersThatWillEnableCharging();
	stubChargerGoodPinHigh();
	publishWokenFromSleep();
	dispatchAllEvents();
	TEST_ASSERT_NULL(batteryChargerEnabledEventArgs);
}

static void stubAllParametersThatWillEnableCharging(void)
{
	stubChargerGoodPinHigh();
	stubTemperatureWithinChargingRange();
	stubBatteryVoltageWithinChargingRange();
	stubChargerGoodPinLow();
}

void test_chargerEnablePin_getWhenChargerGoodPinIsHighWhenOtherParametersAllowCharging_expectChargerEnablePinIsLow(void)
{
	batteryInitialise();
	stubAllParametersThatWillEnableCharging();
	stubChargerGoodPinHigh();
	publishWokenFromSleep();
	LATBbits.LATB3 = 1;
	dispatchAllEvents();
	TEST_ASSERT_FALSE(LATBbits.LATB3);
}

void test_batteryChargerEnabled_eventWhenChargerGoodPinTransitionsFromHighToLowWhenOtherParametersAllowCharging_expectBatteryChargerEnabledEventIsPublished(void)
{
	batteryInitialise();
	stubAllParametersThatWillEnableCharging();
	stubChargerGoodPinHigh();
	dispatchAllEvents();
	batteryChargerEnabledEventArgs = (const struct BatteryChargerEnabled *) 0;

	stubChargerGoodPinLow();
	publishWokenFromSleep();
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(batteryChargerEnabledEventArgs);
}

void test_chargerEnablePin_getWhenChargerGoodPinIsLowWhenOtherParametersAllowCharging_expectChargerEnablePinIsHigh(void)
{
	batteryInitialise();
	stubAllParametersThatWillEnableCharging();
	stubChargerGoodPinLow();
	publishWokenFromSleep();
	LATBbits.LATB3 = 0;
	dispatchAllEvents();
	TEST_ASSERT_TRUE(LATBbits.LATB3);
}

void test_onWokenFromSleep_eventWhenIocInterruptFlagSet_expectOnlyChargerGoodPinFlagIsCleared(void)
{
	batteryInitialise();
	IOCBF = anyByteWithMaskSet(_IOCBF_IOCBF5_MASK);
	uint8_t originalIocbf = IOCBF;
	publishWokenFromSleep();
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_HEX8(originalIocbf & ~_IOCBF_IOCBF5_MASK, IOCBF);
}
