#include <xc.h>

#include "../Platform/NvmSettings.h"
#include "../ApplicationNvmSettings.h"

#include "Door.h"

void doorGetState(struct DoorState *state)
{
	state->actualState = doorState.actualState;
	state->transition = doorState.transition;
	state->flags.isTimeDriven =
		nvmSettings.application.door.mode.isTimeDriven ? 1 : 0;

	state->flags.isSunEventDriven =
		nvmSettings.application.door.mode.isSunEventDriven ? 1 : 0;

	state->flags.isManuallyOverridden =
		(state->flags.isTimeDriven || state->flags.isSunEventDriven)
			? 0
			: 1;
}
