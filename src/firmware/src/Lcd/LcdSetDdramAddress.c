#include <xc.h>
#include <stdint.h>

#include "../NearScheduler.h"

#include "Lcd.h"

static void buggyCompilerWorkaround(void)
{
	static const struct LcdSetAddressTransaction dummy =
	{
		.state = _OMNITARGET
	};
}

void lcdSetDdramAddress(const struct LcdSetAddressTransaction *const transaction)
{
	if (!transaction || lcdState.flags.busy)
		return;

	lcdState.transaction.callback = transaction->callback;
	lcdState.transaction.state = transaction->state;

	static const struct NearSchedule waitForLcdCommand =
	{
		.ticks = MS_TO_TICKS(1),
		.handler = &lcdTransactionCompleted
	};

	lcdWriteCommand(LCD_CMD_SETDDRAMADDRESS | transaction->address);
	nearSchedulerAdd(&waitForLcdCommand);
	lcdState.flags.busy = 1;
}
