#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Main.h"
#include "PowerManagement.h"

#include "TestPowerManagement.h"
#include "NonDeterminism.h"

TEST_FILE("Poll.c")

static volatile uint8_t sleepExecuted;
static volatile uint8_t pie0BeforeSleep;
static volatile uint8_t vregconBeforeSleep;
static volatile uint8_t cpudozeBeforeSleep;

static struct Event onAllEventsDispatchedEvent;
static const struct EventSubscription *onAllEventsDispatched;
static uint8_t numberOfEventPublishesForWokenFromSleep;

void setUp(void)
{
	sleepExecuted = 0;
	onAllEventsDispatched = (const struct EventSubscription *) 0;
	numberOfEventPublishesForWokenFromSleep = 0;
}

void tearDown(void)
{
}

void test_powerManagementInitialise_called_expectPeripheralInterruptsAllowWakeFromSleep(void)
{
	INTCON = anyByteWithMaskClear(_INTCON_PEIE_MASK);
	uint8_t originalIntcon = INTCON;
	powerManagementInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalIntcon | _INTCON_PEIE_MASK, INTCON);
}

void test_powerManagementInitialise_called_expectSubscriptionToAllEventsDispatched(void)
{
	powerManagementInitialise();
	TEST_ASSERT_NOT_NULL(onAllEventsDispatched);
}

void test_onAllEventsDispatched_event_expectDeviceSleepsWhenTimer2IsDisabled(void)
{
	powerManagementInitialise();
	PIE4bits.TMR2IE = 0;
	T2CONbits.ON = 0;
	CPUDOZE = anyByteWithMaskSet(_CPUDOZE_IDLEN_MASK | _CPUDOZE_DOZEN_MASK);

	onAllEventsDispatched->handler(&onAllEventsDispatchedEvent);
	TEST_ASSERT_NOT_EQUAL_MESSAGE(0, sleepExecuted, "SLEEP");
	TEST_ASSERT_BITS_LOW_MESSAGE(
		_CPUDOZE_IDLEN_MASK | _CPUDOZE_DOZEN_MASK,
		cpudozeBeforeSleep,
		"IDLEN / DOZEN");
}

void test_onAllEventsDispatched_event_expectDeviceIdlesWhenTimer2IsEnabled(void)
{
	powerManagementInitialise();
	PIE4bits.TMR2IE = 0;
	T2CONbits.ON = 1;
	CPUDOZE = anyByte();

	onAllEventsDispatched->handler(&onAllEventsDispatchedEvent);
	TEST_ASSERT_NOT_EQUAL_MESSAGE(0, sleepExecuted, "SLEEP");

	TEST_ASSERT_BITS_LOW_MESSAGE(
		_CPUDOZE_DOZEN_MASK,
		cpudozeBeforeSleep,
		"DOZEN");

	TEST_ASSERT_BITS_HIGH_MESSAGE(
		_CPUDOZE_IDLEN_MASK,
		cpudozeBeforeSleep,
		"IDLEN");
}

void test_onAllEventsDispatched_event_expectDeviceEntersLowPowerSleep(void)
{
	powerManagementInitialise();
	VREGCON = anyByteWithMaskClear(_VREGCON_VREGPM_MASK);
	vregconBeforeSleep = VREGCON;
	onAllEventsDispatched->handler(&onAllEventsDispatchedEvent);
	TEST_ASSERT_BIT_HIGH(_VREGCON_VREGPM_POSITION, vregconBeforeSleep);
}

void test_onAllEventsDispatched_event_expectWokenFromSleepIsPublished(void)
{
	powerManagementInitialise();
	onAllEventsDispatched->handler(&onAllEventsDispatchedEvent);
	TEST_ASSERT_EQUAL_UINT8(1, numberOfEventPublishesForWokenFromSleep);
}

void eventSubscribe(const struct EventSubscription *const subscription)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription, "Null subscription");
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription->handler, "Null handler");
	if (subscription->type == ALL_EVENTS_DISPATCHED)
	{
		onAllEventsDispatched = subscription;
		onAllEventsDispatchedEvent.type = subscription->type;
		onAllEventsDispatchedEvent.state = subscription->state;
		onAllEventsDispatchedEvent.args = (void *) 0;
	}
	else
		TEST_FAIL_MESSAGE("Unknown subscription type");
}

void eventPublish(EventType type, const void *const args)
{
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(WOKEN_FROM_SLEEP, type, "Type");
	TEST_ASSERT_NOT_NULL_MESSAGE(args, "Args");
	numberOfEventPublishesForWokenFromSleep++;
}
