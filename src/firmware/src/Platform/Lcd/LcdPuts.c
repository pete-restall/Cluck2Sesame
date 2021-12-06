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

static void __reentrant lcdPutsStateMachine(void *state);

void __reentrant lcdPuts(const struct LcdPutsTransaction *transaction)
{
	if (!transaction || lcdState.flags.isBusy)
		return;

	lcdState.transaction.callback = transaction->callback;
	lcdState.transaction.state = transaction->state;
	lcdState.flags.isBusy = 1;

	lcdPutsStateMachine((void *) transaction->buffer);
}

static void __reentrant lcdPutsStateMachine(void *state)
{
	const char *buffer = (const char *) state;

	if (buffer && *buffer)
	{
		lcdWriteData((uint8_t) *buffer);

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
