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

static void __reentrant lcdPutsStateMachine(void *const state);

void lcdPuts(const struct LcdPutsTransaction *const transaction)
{
	if (!transaction || lcdState.flags.busy)
		return;

	lcdState.transaction.callback = transaction->callback;
	lcdState.transaction.state = transaction->state;
	lcdState.flags.busy = 1;

	lcdPutsStateMachine((void *) transaction->buffer);
}

static void __reentrant lcdPutsStateMachine(void *const state)
{
	const uint8_t *buffer = (const uint8_t *) state;

	if (buffer && *buffer)
	{
		lcdWriteData(*buffer);

		struct NearSchedule waitForLcdCommand =
		{
			.ticks = MS_TO_TICKS(1),
			.handler = &lcdPutsStateMachine,
			.state = (void *) (buffer + 1)
		};

		nearSchedulerAdd(&waitForLcdCommand);
	}
	else
		lcdTransactionCompleted((void *) 0);
}
