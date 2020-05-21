#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "../Platform/Event.h"
#include "../Platform/NvmSettings.h"
#include "../Platform/NearScheduler.h"
#include "../Platform/NvmSettings.h"

#include "Ui.h"

static void uiOnSplashScreenTimeout(void *state);

static const struct NearSchedule uiScreenTimeoutSchedule =
{
	.ticks = MS_TO_TICKS(1000),
	.handler = &uiOnSplashScreenTimeout
};

void uiOnSystemInitialised(const struct Event *event)
{
	memcpy(
		uiState.screen,
		"Cluck2Sesame by \0"
		"lophtware.co.uk ",
		sizeof(uiState.screen));

	uiScreenOn();
	uiState.flags.bits.isInitialSetupRequired = (nvmSettings.application.door.height < 128) ? 1 : 0;
	uiState.flags.bits.isScreenTimeoutDisabled = 1;
	nearSchedulerAdd(&uiScreenTimeoutSchedule);
	uiScreenBlit();
}

static void uiOnSplashScreenTimeout(void *state)
{
	static uint8_t secondsElapsed = 0;
	if (++secondsElapsed == 5)
	{
		uiDateAndTimeEntryScreen();
		uiScreenStartTimeout();
		return;
	}

	nearSchedulerAdd(&uiScreenTimeoutSchedule);
}

void uiOnSystemInitialSetupCompleted(void)
{
	if (!nvmSettingsStore(&uiNvmSettings))
	{
		// TODO: THIS NEEDS TO BE A FAULT...
	}

	uiState.flags.bits.isInitialSetupRequired = 0;
	uiDateAndTimeStatusScreen();
}
