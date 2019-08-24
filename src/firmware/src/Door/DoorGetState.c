#include <xc.h>
#include <stdint.h>

#include "../Platform/NvmSettings.h"
#include "../ApplicationNvmSettings.h"

#include "Door.h"

void doorGetState(struct DoorStateWithContext *state)
{
	state->current = doorState.current;
	state->transition = doorState.transition;
	state->flags.isTimeDriven =
		nvmSettings.application.door.mode.isTimeDriven ? 1 : 0;

	state->flags.isSunEventDriven =
		nvmSettings.application.door.mode.isSunEventDriven ? 1 : 0;

	state->flags.isManuallyOverridden =
		(state->flags.isTimeDriven || state->flags.isSunEventDriven)
			? 0
			: 1;

	state->fault.all =
		doorState.current == DoorState_Fault
			? doorState.aborted.fault.all
			: 0;
}
