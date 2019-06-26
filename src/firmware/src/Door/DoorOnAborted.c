#include <xc.h>
#include <stdint.h>

#include "Door.h"

void onDoorAborted(const struct Event *event)
{
	doorState.current = DoorState_Fault;
	doorState.transition = DoorTransition_Unchanged;
}
