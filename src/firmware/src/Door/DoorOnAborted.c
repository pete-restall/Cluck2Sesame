#include <xc.h>
#include <stdint.h>

#include "Door.h"

void doorOnAborted(const struct Event *event)
{
	doorState.current = DoorState_Fault;
	doorState.transition = DoorTransition_Unchanged;
// TODO: DISABLE MOTOR...
}
