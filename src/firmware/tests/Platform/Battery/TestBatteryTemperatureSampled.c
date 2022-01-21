#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/Battery.h"
#include "Platform/Temperature.h"

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

void test_batteryChargerEnabled_eventWhenTemperatureIsOver30CelsiusWhenOtherParametersAllowCharging_expectNoBatteryChargerEnabledEventIsPublished(void)
{
	batteryInitialise();
	stubAllParametersThatWillEnableCharging();
	batteryChargerEnabledEventArgs = (const struct BatteryChargerEnabled *) 0;
	for (int16_t tooHot = CELSIUS(30.1); tooHot <= CELSIUS(50); tooHot++)
	{
		stubTemperatureOf(tooHot);
		dispatchAllEvents();
		TEST_ASSERT_NULL(batteryChargerEnabledEventArgs);
	}
}

void test_chargerEnablePin_getWhenTemperatureIsOver30CelsiusWhenOtherParametersAllowCharging_expectChargerEnablePinIsLow(void)
{
	batteryInitialise();
	stubAllParametersThatWillEnableCharging();
	for (int16_t tooHot = CELSIUS(30.1); tooHot <= CELSIUS(50); tooHot++)
	{
		stubTemperatureOf(tooHot);
		LATBbits.LATB3 = 1;
		dispatchAllEvents();
		TEST_ASSERT_FALSE(LATBbits.LATB3);
	}
}

void test_batteryChargerEnabled_eventWhenTemperatureIsBelow5CelsiusWhenOtherParametersAllowCharging_expectNoBatteryChargerEnabledEventIsPublished(void)
{
	batteryInitialise();
	stubAllParametersThatWillEnableCharging();
	batteryChargerEnabledEventArgs = (const struct BatteryChargerEnabled *) 0;
	for (int16_t tooCold = CELSIUS(4.9); tooCold >= CELSIUS(-25); tooCold--)
	{
		stubTemperatureOf(tooCold);
		dispatchAllEvents();
		TEST_ASSERT_NULL(batteryChargerEnabledEventArgs);
	}
}

void test_chargerEnablePin_getWhenTemperatureIsBelow5CelsiusWhenOtherParametersAllowCharging_expectChargerEnablePinIsLow(void)
{
	batteryInitialise();
	stubAllParametersThatWillEnableCharging();
	for (int16_t tooCold = CELSIUS(4.9); tooCold >= CELSIUS(-25); tooCold--)
	{
		stubTemperatureOf(tooCold);
		LATBbits.LATB3 = 1;
		dispatchAllEvents();
		TEST_ASSERT_FALSE(LATBbits.LATB3);
	}
}

void test_batteryChargerEnabled_eventWhenTemperatureIsBetween5And30CelsiusWhenOtherParametersAllowCharging_expectBatteryChargerEnabledEventIsPublished(void)
{
	batteryInitialise();
	stubAllParametersThatWillEnableCharging();
	for (int16_t justRight = CELSIUS(5); justRight <= CELSIUS(30); justRight++)
	{
		stubTemperatureOf(CELSIUS(-10));
		dispatchAllEvents();
		batteryChargerEnabledEventArgs = (const struct BatteryChargerEnabled *) 0;
		stubTemperatureOf(justRight);
		dispatchAllEvents();
		TEST_ASSERT_NOT_NULL(batteryChargerEnabledEventArgs);
	}
}

void test_chargerEnablePin_getWhenTemperatureIsBetween5And30CelsiusWhenOtherParametersAllowCharging_expectChargerEnablePinIsHigh(void)
{
	batteryInitialise();
	stubAllParametersThatWillEnableCharging();
	for (int16_t justRight = CELSIUS(5); justRight <= CELSIUS(30); justRight++)
	{
		stubTemperatureOf(justRight);
		LATBbits.LATB3 = 0;
		dispatchAllEvents();
		TEST_ASSERT_TRUE(LATBbits.LATB3);
	}
}

void test_batteryChargerDisabled_eventWhenTemperatureTransitionsBelow5CelsiusWhenOtherParametersAllowCharging_expectBatteryChargerDisabledEventIsPublished(void)
{
	batteryInitialise();
	for (int16_t tooCold = CELSIUS(4.9); tooCold >= CELSIUS(-25); tooCold--)
	{
		stubAllParametersThatWillEnableCharging();
		dispatchAllEvents();
		stubTemperatureOf(tooCold);
		batteryChargerDisabledEventArgs = (const struct BatteryChargerDisabled *) 0;
		dispatchAllEvents();
		TEST_ASSERT_NOT_NULL(batteryChargerDisabledEventArgs);
	}
}

void test_batteryChargerDisabled_eventWhenTemperatureTransitionsAbove30CelsiusWhenOtherParametersAllowCharging_expectBatteryChargerDisabledEventIsPublished(void)
{
	batteryInitialise();
	for (int16_t tooHot = CELSIUS(30.1); tooHot <= CELSIUS(50); tooHot++)
	{
		stubAllParametersThatWillEnableCharging();
		dispatchAllEvents();
		stubTemperatureOf(tooHot);
		batteryChargerDisabledEventArgs = (const struct BatteryChargerDisabled *) 0;
		dispatchAllEvents();
		TEST_ASSERT_NOT_NULL(batteryChargerDisabledEventArgs);
	}
}
