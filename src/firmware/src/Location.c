#include <xc.h>

#include "../Platform/Event.h"
#include "../Platform/NvmSettings.h"

#include "Location.h"

static void buggyCompilerWorkaround(void)
{
	static const struct LocationChanged dummy =
	{
		.location = _OMNITARGET
	};
}

static void onNvmSettingsChanged(const struct Event *event);

static struct Location location;

void locationInitialise(void)
{
	static const struct EventSubscription onNvmSettingsChangedSubscription =
	{
		.type = NVM_SETTINGS_CHANGED,
		.handler = &onNvmSettingsChanged,
		.state = (void *) 0
	};

	eventSubscribe(&onNvmSettingsChangedSubscription);

	location.latitudeOffset = nvmSettings.application.location.latitudeOffset;
	location.longitudeOffset = nvmSettings.application.location.longitudeOffset;
}

static void onNvmSettingsChanged(const struct Event *event)
{
	if (nvmSettings.application.location.latitudeOffset == location.latitudeOffset &&
		nvmSettings.application.location.longitudeOffset == location.longitudeOffset)
		return;

	static const struct LocationChanged locationChangedEventArgs = { .location = &location };
	location.latitudeOffset = nvmSettings.application.location.latitudeOffset;
	location.longitudeOffset = nvmSettings.application.location.longitudeOffset;
	eventPublish(LOCATION_CHANGED, &locationChangedEventArgs);
}
