#include <xc.h>

#include "Lcd.h"

void lcdTransactionCompleted(void *unused)
{
	lcdState.flags.isBusy = 0;
	if (lcdState.transaction.callback)
		lcdState.transaction.callback(lcdState.transaction.state);
}
