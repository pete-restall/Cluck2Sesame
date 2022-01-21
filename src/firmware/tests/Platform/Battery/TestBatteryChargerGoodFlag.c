#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/Battery.h"

#include "BatteryFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Battery.c")

void onBeforeTest(void)
{
	batteryFixtureSetUp();
}

void onAfterTest(void)
{
	batteryFixtureTearDown();
}

void test_batteryChargerEnabled_eventWhenChargerGoodPinIsHighWhenOtherParametersAllowCharging_expectNoBatteryChargerEnabledEventIsPublished(void)
{
	batteryInitialise();
	stubAllParametersThatWillEnableCharging();
	stubChargerGoodPinHigh();
	batteryChargerEnabledEventArgs = (const struct BatteryChargerEnabled *) 0;
	dispatchAllEvents();
	TEST_ASSERT_NULL(batteryChargerEnabledEventArgs);
}

void test_chargerEnablePin_getWhenChargerGoodPinIsHighWhenOtherParametersAllowCharging_expectChargerEnablePinIsLow(void)
{
	batteryInitialise();
	stubAllParametersThatWillEnableCharging();
	stubChargerGoodPinHigh();
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
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(batteryChargerEnabledEventArgs);
}

void test_chargerEnablePin_getWhenChargerGoodPinIsLowWhenOtherParametersAllowCharging_expectChargerEnablePinIsHigh(void)
{
	batteryInitialise();
	stubAllParametersThatWillEnableCharging();
	stubChargerGoodPinHigh();
	dispatchAllEvents();

	stubChargerGoodPinLow();
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

void test_onWokenFromSleep_eventWhenIocInterruptFlagIsNotSet_expectChargingStateIsStillReEvaluated(void)
{
	batteryInitialise();
	stubAllParametersThatWillEnableCharging();
	dispatchAllEvents();
	stubChargerGoodPinHigh();
	IOCBF = 0;
	batteryChargerDisabledEventArgs = (const struct BatteryChargerDisabled *) 0;
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(batteryChargerDisabledEventArgs);
}

void test_batteryChargerDisabled_eventWhenChargerGoodPinTransitionsFromLowToHighWhenOtherParametersAllowCharging_expectBatteryChargerDisabledEventIsPublished(void)
{
	batteryInitialise();
	stubAllParametersThatWillEnableCharging();
	dispatchAllEvents();
	stubChargerGoodPinHigh();
	batteryChargerDisabledEventArgs = (const struct BatteryChargerDisabled *) 0;
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(batteryChargerDisabledEventArgs);
}
