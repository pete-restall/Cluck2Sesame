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

static void lcdPutsStateMachine(void *state);

void lcdPuts(const struct LcdPutsTransaction *transaction)
{
	if (!transaction || lcdState.flags.isBusy)
		return;

	lcdState.transaction.callback = transaction->callback;
	lcdState.transaction.state = transaction->state;
	lcdState.flags.isBusy = 1;

	lcdPutsStateMachine((void *) transaction->buffer);
}

static void lcdPutsStateMachine(void *state)
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
