#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/PeriodicMonitor.h"

#include "PeriodicMonitorFixture.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Event.c")
TEST_FILE("Platform/PeriodicMonitor.c")

void onBeforeTest(void)
{
	periodicMonitorFixtureSetUp();
}

void onAfterTest(void)
{
	periodicMonitorFixtureTearDown();
}

void test_timeChanged_onPublishedWithNonFourMinuteIntervalSchedule_expectMonitoredParamtersSampledEventIsNotPublished(void)
{
	mockOnMonitoredParametersSampled();

	struct Time now =
	{
		.hour = anyByteLessThan(24),
		.second = anyByteLessThan(60)
	};

	for (uint8_t minute = 0; minute < 60; minute++)
	{
		uint8_t offset = minute % 4;
		if (offset == 1)
			continue;

		now.minute = minute;
		publishTimeChanged(&now);
		dispatchAllEvents();
		TEST_ASSERT_EQUAL_UINT8(0, monitoredParametersSampledCalls);
	}
}

void test_timeChanged_onPublishedWithFourMinuteIntervalSchedule_expectMonitoredParamtersSampledEventIsPublished(void)
{
	mockOnMonitoredParametersSampled();

	struct Time now =
	{
		.hour = anyByteLessThan(24),
		.second = anyByteLessThan(60)
	};

	for (uint8_t i = 1, minute = 1; minute < 60; minute += 4, i++)
	{
		now.minute = minute;
		publishTimeChanged(&now);
		dispatchAllEvents();
		TEST_ASSERT_EQUAL_UINT8(i, monitoredParametersSampledCalls);
	}
}
