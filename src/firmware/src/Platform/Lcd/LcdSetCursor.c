#include <xc.h>
#include <stdint.h>

#include "../NearScheduler.h"

#include "Lcd.h"

static void buggyCompilerWorkaround(void)
{
	static const struct LcdSetCursorTransaction dummy =
	{
		.state = _OMNITARGET
	};
}

void lcdSetCursor(const struct LcdSetCursorTransaction *transaction)
{
	if (!transaction || lcdState.flags.isBusy)
		return;

	lcdState.transaction.callback = transaction->callback;
	lcdState.transaction.state = transaction->state;

	static const struct NearSchedule waitForLcdCommand =
	{
		.ticks = MS_TO_TICKS(1),
		.handler = &lcdTransactionCompleted
	};

	lcdWriteCommand(LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_ON | (transaction->on ? 3 : 0));
	nearSchedulerAdd(&waitForLcdCommand);
	lcdState.flags.isBusy = 1;
}
