#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/NvmSettings.h"
#include "Platform/CalibrationMode.h"

#include "Mock_PowerManagement.h"
#include "Mock_PeriodicMonitor.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"
#include "../../NvmSettingsFixture.h"

TEST_FILE("Platform/CalibrationMode.c")

static struct Event onMonitoredParametersSampledEvent;
static const struct EventSubscription *onMonitoredParametersSampled;

static struct Event onWokenFromSleepEvent;
static const struct EventSubscription *onWokenFromSleep;

const struct Event eventEmptyArgs = { };

static const union NvmSettings withCalibrationRequired =
{
	.platform =
	{
		.flags = { .bits = { .isCalibrationRequired = 1 } }
	}
};

static const union NvmSettings withoutCalibrationRequired =
{
	.platform =
	{
		.flags = { .bits = { .isCalibrationRequired = 0 } }
	}
};

void onBeforeTest(void)
{
	onMonitoredParametersSampled = (const struct EventSubscription *) 0;
	onWokenFromSleep = (const struct EventSubscription *) 0;
}

void onAfterTest(void)
{
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectSubscriptionToMonitoredParametersSampled(void)
{
	stubNvmSettings(&withCalibrationRequired);
	calibrationModeInitialise();
	TEST_ASSERT_NOT_NULL(onMonitoredParametersSampled);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectNoSubscriptionToMonitoredParametersSampled(void)
{
	stubNvmSettings(&withoutCalibrationRequired);
	calibrationModeInitialise();
	TEST_ASSERT_NULL(onMonitoredParametersSampled);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectSubscriptionToWokenFromSleep(void)
{
	stubNvmSettings(&withCalibrationRequired);
	calibrationModeInitialise();
	TEST_ASSERT_NOT_NULL(onWokenFromSleep);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectNoSubscriptionToWokenFromSleep(void)
{
	stubNvmSettings(&withoutCalibrationRequired);
	calibrationModeInitialise();
	TEST_ASSERT_NULL(onWokenFromSleep);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectClockReferenceModuleIsEnabled(void)
{
	stubNvmSettings(&withCalibrationRequired);
	PMD0 = anyByteWithMaskSet(_PMD0_CLKRMD_MASK);
	uint8_t originalPmd0 = PMD0;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPmd0 & ~_PMD0_CLKRMD_MASK, PMD0);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectClockReferenceModuleIsDisabled(void)
{
	stubNvmSettings(&withoutCalibrationRequired);
	PMD0 = anyByteWithMaskClear(_PMD0_CLKRMD_MASK);
	uint8_t originalPmd0 = PMD0;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPmd0 | _PMD0_CLKRMD_MASK, PMD0);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectIcspPinsAreDigital(void)
{
	stubNvmSettings(&withCalibrationRequired);

	static const uint8_t usedPins = _ANSELB_ANSB6_MASK | _ANSELB_ANSB7_MASK;
	ANSELB = anyByteWithMaskSet(usedPins);
	uint8_t originalAnselb = ANSELB;

	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalAnselb & ~usedPins, ANSELB);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectIcspPinsAreDigital(void)
{
	stubNvmSettings(&withoutCalibrationRequired);

	static const uint8_t usedPins = _ANSELB_ANSB6_MASK | _ANSELB_ANSB7_MASK;
	ANSELB = anyByteWithMaskSet(usedPins);
	uint8_t originalAnselb = ANSELB;

	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalAnselb & ~usedPins, ANSELB);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectIcspPgcPinIsOutput(void)
{
	stubNvmSettings(&withCalibrationRequired);
	TRISB = anyByteWithMaskSet(_TRISB_TRISB6_MASK);
	uint8_t originalTrisb = TRISB & ~_TRISB_TRISB7_MASK;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalTrisb & ~_TRISB_TRISB6_MASK, TRISB & ~_TRISB_TRISB7_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectIcspPgcPinIsOutput(void)
{
	stubNvmSettings(&withoutCalibrationRequired);
	TRISB = anyByteWithMaskSet(_TRISB_TRISB6_MASK);
	uint8_t originalTrisb = TRISB & ~_TRISB_TRISB7_MASK;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalTrisb & ~_TRISB_TRISB6_MASK, TRISB & ~_TRISB_TRISB7_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectIcspPgcPinIsOpenDrain(void)
{
	stubNvmSettings(&withCalibrationRequired);
	ODCONB = anyByteWithMaskClear(_ODCONB_ODCB6_MASK);
	uint8_t originalOdconb = ODCONB;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalOdconb | _ODCONB_ODCB6_MASK, ODCONB);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectIcspPgcPinIsNotOpenDrain(void)
{
	stubNvmSettings(&withoutCalibrationRequired);
	ODCONB = anyByteWithMaskSet(_ODCONB_ODCB6_MASK);
	uint8_t originalOdconb = ODCONB;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalOdconb & ~_ODCONB_ODCB6_MASK, ODCONB);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectIcspPgcPinIsMappedToReferenceClockOutput(void)
{
	stubNvmSettings(&withCalibrationRequired);
	RB6PPS = anyByteExcept(0x1b);
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(0x1b, RB6PPS);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectIcspPgcPinIsNotMappedToAnyPeripheralOutput(void)
{
	stubNvmSettings(&withoutCalibrationRequired);
	RB6PPS = anyByteExcept(0);
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(0, RB6PPS);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectIcspPgdPinIsInput(void)
{
	stubNvmSettings(&withCalibrationRequired);
	TRISB = anyByteWithMaskClear(_TRISB_TRISB7_MASK);
	uint8_t originalTrisb = TRISB & ~_TRISB_TRISB6_MASK;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalTrisb | _TRISB_TRISB7_MASK, TRISB & ~_TRISB_TRISB6_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectIcspPgdPinIsOutput(void)
{
	stubNvmSettings(&withoutCalibrationRequired);
	TRISB = anyByteWithMaskSet(_TRISB_TRISB7_MASK);
	uint8_t originalTrisb = TRISB & ~_TRISB_TRISB6_MASK;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalTrisb & ~_TRISB_TRISB7_MASK, TRISB & ~_TRISB_TRISB6_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectIcspPgdPinIsMappedToUart1TxAndRx(void)
{
	stubNvmSettings(&withCalibrationRequired);
	RB7PPS = anyByteExcept(0x10);
	RX1DTPPS = anyByte();
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x10, RB7PPS, "TX");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x0f, RX1DTPPS, "RX");
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectIcspPgdPinIsNotMappedToAnyPeripheralOutput(void)
{
	stubNvmSettings(&withoutCalibrationRequired);
	RB7PPS = anyByteExcept(0);
	RX1DTPPS = anyByteExcept(0);
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, RB7PPS, "TX");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, RX1DTPPS, "RX");
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectIcspPinsAreLow(void)
{
	stubNvmSettings(&withCalibrationRequired);
	LATB = anyByteWithMaskSet(_LATB_LATB6_MASK | _LATB_LATB7_MASK);
	uint8_t originalLatb = LATB;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalLatb & ~(_LATB_LATB6_MASK | _LATB_LATB7_MASK), LATB);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectIcspPinsAreLow(void)
{
	stubNvmSettings(&withoutCalibrationRequired);
	LATB = anyByteWithMaskSet(_LATB_LATB6_MASK | _LATB_LATB7_MASK);
	uint8_t originalLatb = LATB;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalLatb & ~(_LATB_LATB6_MASK | _LATB_LATB7_MASK), LATB);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectReferenceClockSourceIs32768HzCrystal(void)
{
	stubNvmSettings(&withCalibrationRequired);
	CLKRCLK = anyByte();
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(0b0101 << _CLKRCLK_CLKRCLK_POSITION, CLKRCLK & _CLKRCLK_CLKRCLK_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectReferenceClockDutyCycleIs50Percent(void)
{
	stubNvmSettings(&withCalibrationRequired);
	CLKRCON = anyByte();
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(0b10 << _CLKRCON_CLKRDC_POSITION, CLKRCON & _CLKRCON_CLKRDC_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectReferenceClockIsNotDivided(void)
{
	stubNvmSettings(&withCalibrationRequired);
	CLKRCON = anyByte();
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(0b000 << _CLKRCON_CLKRDIV_POSITION, CLKRCON & _CLKRCON_CLKRDIV_MASK);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectReferenceClockIsNotEnabled(void)
{
	stubNvmSettings(&withCalibrationRequired);
	CLKRCON = anyByteWithMaskSet(_CLKRCON_CLKREN_MASK);
	calibrationModeInitialise();
	TEST_ASSERT_BIT_LOW(_CLKRCON_CLKREN_POSITION, CLKRCON);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsRequired_expectUart1ModuleIsEnabled(void)
{
	stubNvmSettings(&withCalibrationRequired);
	PMD4 = anyByteWithMaskSet(_PMD4_UART1MD_MASK);
	uint8_t originalPmd4 = PMD4;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPmd4 & ~_PMD4_UART1MD_MASK, PMD4);
}

void test_calibrationModeInitialise_calledWhenCalibrationIsNotRequired_expectUart1ModuleIsDisabled(void)
{
	stubNvmSettings(&withoutCalibrationRequired);
	PMD4 = anyByteWithMaskClear(_PMD4_UART1MD_MASK);
	uint8_t originalPmd4 = PMD4;
	calibrationModeInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPmd4 | _PMD4_UART1MD_MASK, PMD4);
}

// TODO: NEXT TESTS WILL ENSURE THAT THE UART IS CORRECTLY SETUP...

void test_onMonitoredParametersSampled_called_expectSOMETHINGBLAHBLAHBLAH(void)
{
	// TODO: ONLY REQUIRED AT THE MOMENT TO PREVENT FUNCTIONS BEING OPTIMISED AWAY - MAKE A PROPER TEST OUT OF THIS...
	calibrationModeInitialise();
	//onMonitoredParametersSampledEvent->args = ...
	onMonitoredParametersSampled->handler(&onMonitoredParametersSampledEvent);
	TEST_ASSERT_TRUE(1);
}

void eventSubscribe(const struct EventSubscription *subscription)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription, "Null subscription");
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription->handler, "Null handler");
	if (subscription->type == MONITORED_PARAMETERS_SAMPLED)
	{
		onMonitoredParametersSampled = subscription;
		onMonitoredParametersSampledEvent.type = subscription->type;
		onMonitoredParametersSampledEvent.state = subscription->state;
		onMonitoredParametersSampledEvent.args = (void *) 0;
	}
	else if (subscription->type == WOKEN_FROM_SLEEP)
	{
		onWokenFromSleep = subscription;
		onWokenFromSleepEvent.type = subscription->type;
		onWokenFromSleepEvent.state = subscription->state;
		onWokenFromSleepEvent.args = (void *) 0;
	}
	else
	{
		TEST_FAIL_MESSAGE("Unknown subscription type");
	}
}
