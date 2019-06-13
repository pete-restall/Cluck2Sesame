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

void lcdSetDdramAddress(const struct LcdSetAddressTransaction *transaction)
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

	lcdWriteCommand(LCD_CMD_SETDDRAMADDRESS | transaction->address);
	nearSchedulerAdd(&waitForLcdCommand);
	lcdState.flags.isBusy = 1;
}
