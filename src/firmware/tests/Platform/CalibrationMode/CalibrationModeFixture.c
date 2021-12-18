#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/NvmSettings.h"
#include "Platform/PowerManagement.h"
#include "Platform/CalibrationMode.h"

#include "../../NonDeterminism.h"
#include "../../Fixture.h"
#include "../../NvmSettingsFixture.h"

#include "CalibrationModeFixture.h"

static struct Event onWokenFromSleepEvent;
static const struct EventSubscription *onWokenFromSleep;

const struct Event eventEmptyArgs = { };

static void buggyCompilerWorkaround(void)
{
	if (0)
		onWokenFromSleep->handler(&onWokenFromSleepEvent);
}

void calibrationModeFixtureSetUp(void)
{
	onWokenFromSleep = (const struct EventSubscription *) 0;
	buggyCompilerWorkaround();
}

void calibrationModeFixtureTearDown(void)
{
}

void stubNvmSettingsWithCalibrationRequired(void)
{
	static const union NvmSettings withCalibrationRequired =
	{
		.platform =
		{
			.flags = { .bits = { .isCalibrationRequired = 1 } }
		}
	};

	stubNvmSettings(&withCalibrationRequired);
}

void stubNvmSettingsWithoutCalibrationRequired(void)
{
	static const union NvmSettings withoutCalibrationRequired =
	{
		.platform =
		{
			.flags = { .bits = { .isCalibrationRequired = 0 } }
		}
	};

	stubNvmSettings(&withoutCalibrationRequired);
}

void eventSubscribe(const struct EventSubscription *subscription)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription, "Null subscription");
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription->handler, "Null handler");
	if (subscription->type == WOKEN_FROM_SLEEP)
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

void assertWokenFromSleepSubscription(void)
{
	TEST_ASSERT_NOT_NULL(onWokenFromSleep);
}

void assertNoWokenFromSleepSubscription(void)
{
	TEST_ASSERT_NULL(onWokenFromSleep);
}
