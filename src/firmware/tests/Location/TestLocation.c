#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"

#include "Location.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"
#include "../../NvmSettingsFixture.h"

TEST_FILE("Location.c")

const struct Event eventEmptyArgs = { };

static void onLocationChanged(const struct Event *event);

static const struct LocationChanged *locationChangedEventArgs;

static union ApplicationNvmSettings appNvmSettings;

void onBeforeTest(void)
{
	eventInitialise();

	static const struct EventSubscription onLocationChangedSubscription =
	{
		.type = LOCATION_CHANGED,
		.handler = &onLocationChanged,
		.state = (void *) 0
	};

	eventSubscribe(&onLocationChangedSubscription);
	locationChangedEventArgs = (const struct LocationChanged *) 0;

	appNvmSettings.location.longitudeOffset = (int8_t) anyByte();
	appNvmSettings.location.latitudeOffset = (int8_t) anyByte();
	stubNvmApplicationSettings(&appNvmSettings);
}

static void onLocationChanged(const struct Event *event)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(event, "Null event !");
	locationChangedEventArgs = (const struct LocationChanged *) event->args;
	TEST_ASSERT_NOT_NULL_MESSAGE(locationChangedEventArgs, "Null event args !");
	TEST_ASSERT_NOT_NULL_MESSAGE(locationChangedEventArgs->location, "Null location !");
}

void onAfterTest(void)
{
}

void test_locationInitialise_expectNoLocationChangedEventIsPublished(void)
{
	locationInitialise();
	dispatchAllEvents();
	TEST_ASSERT_NULL(locationChangedEventArgs);
}

void test_onNvmSettingsChanged_eventPublishedWithSameLongitudeAndLatitude_expectNoLocationChangedEventIsPublished(void)
{
	locationInitialise();
	eventPublish(NVM_SETTINGS_CHANGED, &eventEmptyArgs);
	dispatchAllEvents();
	TEST_ASSERT_NULL(locationChangedEventArgs);
}

void test_onNvmSettingsChanged_eventPublishedWithDifferentLongitudeAndSameLatitude_expectLocationChangedEventIsPublishedWithUpdatedLongitude(void)
{
	locationInitialise();
	appNvmSettings.location.longitudeOffset = (int8_t) anyByteExcept((uint8_t) appNvmSettings.location.longitudeOffset);
	stubNvmApplicationSettings(&appNvmSettings);
	eventPublish(NVM_SETTINGS_CHANGED, &eventEmptyArgs);
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(locationChangedEventArgs);
	TEST_ASSERT_EQUAL_INT8(appNvmSettings.location.longitudeOffset, locationChangedEventArgs->location->longitudeOffset);
}

void test_onNvmSettingsChanged_eventPublishedWithDifferentLongitudeAndSameLatitude_expectLocationChangedEventIsPublishedWithPreviousLatitude(void)
{
	locationInitialise();
	appNvmSettings.location.longitudeOffset = (int8_t) anyByteExcept((uint8_t) appNvmSettings.location.longitudeOffset);
	stubNvmApplicationSettings(&appNvmSettings);
	eventPublish(NVM_SETTINGS_CHANGED, &eventEmptyArgs);
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(locationChangedEventArgs);
	TEST_ASSERT_EQUAL_INT8(appNvmSettings.location.latitudeOffset, locationChangedEventArgs->location->latitudeOffset);
}

void test_onNvmSettingsChanged_eventPublishedWithSameLongitudeAndDifferentLatitude_expectLocationChangedEventIsPublishedWithUpdatedLatitude(void)
{
	locationInitialise();
	appNvmSettings.location.latitudeOffset = (int8_t) anyByteExcept((uint8_t) appNvmSettings.location.latitudeOffset);
	stubNvmApplicationSettings(&appNvmSettings);
	eventPublish(NVM_SETTINGS_CHANGED, &eventEmptyArgs);
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(locationChangedEventArgs);
	TEST_ASSERT_EQUAL_INT8(appNvmSettings.location.latitudeOffset, locationChangedEventArgs->location->latitudeOffset);
}

void test_onNvmSettingsChanged_eventPublishedWithSameLongitudeAndDifferentLatitude_expectLocationChangedEventIsPublishedWithPreviousLongitude(void)
{
	locationInitialise();
	appNvmSettings.location.latitudeOffset = (int8_t) anyByteExcept((uint8_t) appNvmSettings.location.latitudeOffset);
	stubNvmApplicationSettings(&appNvmSettings);
	eventPublish(NVM_SETTINGS_CHANGED, &eventEmptyArgs);
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(locationChangedEventArgs);
	TEST_ASSERT_EQUAL_INT8(appNvmSettings.location.longitudeOffset, locationChangedEventArgs->location->longitudeOffset);
}

void test_onNvmSettingsChanged_eventPublishedWithDifferentLongitudeAndLatitude_expectLocationChangedEventIsPublishedWithUpdatedValues(void)
{
	locationInitialise();
	appNvmSettings.location.longitudeOffset = (int8_t) anyByteExcept((uint8_t) appNvmSettings.location.longitudeOffset);
	appNvmSettings.location.latitudeOffset = (int8_t) anyByteExcept((uint8_t) appNvmSettings.location.latitudeOffset);
	stubNvmApplicationSettings(&appNvmSettings);
	eventPublish(NVM_SETTINGS_CHANGED, &eventEmptyArgs);
	dispatchAllEvents();
	TEST_ASSERT_NOT_NULL(locationChangedEventArgs);
	TEST_ASSERT_EQUAL_INT8_MESSAGE(appNvmSettings.location.longitudeOffset, locationChangedEventArgs->location->longitudeOffset, "LONG");
	TEST_ASSERT_EQUAL_INT8_MESSAGE(appNvmSettings.location.latitudeOffset, locationChangedEventArgs->location->latitudeOffset, "LAT");
}
