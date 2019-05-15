#include <xc.h>
#include <stdint.h>

#include "../NearScheduler.h"

#include "Lcd.h"

static void buggyCompilerWorkaround(void)
{
	static const struct LcdPutsTransaction dummy =
	{
		.buffer = _OMNITARGET,
		.state = _OMNITARGET
	};
}

static void lcdPutsStateMachine(void *const state);

void lcdPuts(const struct LcdPutsTransaction *const transaction)
{
	static struct LcdPutsTransaction state;

	// TODO: MULTIPLE CALLS ?
	state.buffer = transaction->buffer;
	state.callback = transaction->callback;
	state.state = transaction->state;
	lcdPutsStateMachine(&state);
}

static void lcdPutsStateMachine(void *const state)
{
	struct LcdPutsTransaction *putsState = (struct LcdPutsTransaction *) state;

	struct NearSchedule waitForLcdCommand =
	{
		.ticks = MS_TO_TICKS(1),
		.handler = &lcdPutsStateMachine,
		.state = state
	};

	if (*putsState->buffer)
	{
		lcdWriteData(*putsState->buffer);
		nearSchedulerAdd(&waitForLcdCommand);
		putsState->buffer = putsState->buffer + 1;
	}
	else
		putsState->callback(putsState->state);
}
