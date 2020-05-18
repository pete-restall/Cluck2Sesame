#ifndef __CLUCK3SESAME_SRC_DOOR_DOOR_H
#define __CLUCK3SESAME_SRC_DOOR_DOOR_H
#include "../Platform/Event.h"
#include "../Door.h"

#define SPOOL_CIRCUMFERENCE_MM (20 * 3.1415926535)
#define GEAR_RATIO 65
#define ENCODER_PULSES_PER_TURN 11
#define MM_TO_ENCODER_PULSES(mm) ((mm) / SPOOL_CIRCUMFERENCE_MM * GEAR_RATIO * ENCODER_PULSES_PER_TURN)

#define MANUAL_MOVEMENT_LIMIT MM_TO_ENCODER_PULSES(1000)

#define FIND_BOTTOM_LOWERING ((int16_t) (MM_TO_ENCODER_PULSES(-100) + 0.5))
#define FIND_BOTTOM_RAISING  ((int16_t) (MM_TO_ENCODER_PULSES(1000) + 0.5))
#define FIND_BOTTOM_THRESHOLD ((int16_t) (MM_TO_ENCODER_PULSES(2) + 0.5))
#define MAX_FIND_BOTTOM_ITERATIONS 10

#define DOOR_JAMMED 1
#define DOOR_REVERSED 2
#define LINE_SNAPPED 4
#define LINE_TOO_LONG 8
#define ENCODER_BROKEN 16

struct DoorStateInternal
{
	enum DoorState current;
	enum DoorTransition transition;
	struct DoorOpened opened;
	struct DoorClosed closed;
	struct DoorAborted aborted;
	uint8_t findBottomIterations;
};

extern struct DoorStateInternal doorState;

extern void doorOnOpenScheduleActioned(const struct Event *event);
extern void doorStartOpening(
	enum DoorState motorEnabledState,
	enum DoorState motorDisabledState);

extern void doorStartFindingBottom(
	enum DoorState motorEnabledState,
	enum DoorState motorDisabledState);

extern void doorOnCloseScheduleActioned(const struct Event *event);
extern void doorStartClosing(
	enum DoorState motorEnabledState,
	enum DoorState motorDisabledState);

extern void doorOnMotorStopped(const struct Event *event);
extern void doorOnMotorEnabled(const struct Event *event);

#endif
