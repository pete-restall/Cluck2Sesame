#include <xc.h>
#include <stdint.h>

#include "Door.h"

void onDoorAborted(const struct Event *event)
{
	doorState.actualState = DoorActualState_Fault;
	doorState.transition = DoorTransition_Unchanged;
}
