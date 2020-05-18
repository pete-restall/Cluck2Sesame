#include <xc.h>
#include <stdint.h>

#include "../Platform/Motor.h"

#include "Door.h"

void doorManualStartOpening(void)
{
	motorEnable();
	doorStartOpening(
		DoorState_ManualOpening,
		DoorState_ManualOpening_WaitingForEnabledMotor);
	// TODO: THE MOTOR NEEDS TO MOVE AN AMOUNT THAT IS NOT THE SAME AS THE HEIGHT...
	// TODO: TEST WHAT HAPPENS WHEN MOTOR CURRENT LIMIT KICKS IN - FAULT SHOULD BE 'OVERRIDDEN' AS THE USER MAY NEED TO MOVE THE SPOOL TO FIX THE PROBLEM.
	// TODO: WHAT IF THE MOTOR IS ALREADY TURNING AS PART OF ANOTHER STATE ???  MAYBE IN THE MIDDLE OF AN OPEN OR CLOSE ?  REVERSING ?
	// TODO: WHAT IF THE MOTOR IS ALREADY TURNING AS PART OF A MANUAL STATE (IE. QUICK SUCCESSIVE BUTTON PRESSES) ?
	// TODO: DO NOT MODIFY 'doorState.transition' SINCE THIS WILL BE UPDATED BY THE OPEN / CLOSE SCHEDULE, THEN AT LEAST IT WILL WORK PROPERLY WHEN WE SWITCH BACK FROM MANUAL CONTROL...
}

void doorManualStartClosing(void)
{
	// TODO: ALONG SIMILAR LINES AS THE MANUAL OPEN...
	motorEnable();
	doorStartClosing(
		DoorState_ManualClosing,
		DoorState_ManualClosing_WaitingForEnabledMotor);
}

void doorManualStop(void)
{
	motorOff();
}
