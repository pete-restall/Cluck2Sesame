#include <xc.h>
#include <stdint.h>

#include "../NearScheduler.h"

#include "Lcd.h"

static void lcdConfigureStateMachine(void *state);

void lcdConfigure(void)
{
	static const struct NearSchedule waitForLcdToStabilise =
	{
		.ticks = MS_TO_TICKS(48),
		.handler = &lcdConfigureStateMachine
	};

	nearSchedulerAdd(&waitForLcdToStabilise);
	PWM5CONbits.PWM5EN = 1;
	lcdState.flags.isBusy = 1;
}

static void lcdConfigureStateMachine(void *state)
{
	static const struct LcdEnabled emptyEventArgs = { };

	struct NearSchedule waitForLcdCommand =
	{
		.ticks = MS_TO_TICKS(8),
		.handler = &lcdConfigureStateMachine,
		.state = (void *) (((int) state) + 1)
	};

	switch ((int) state)
	{
		case 0:
		case 1:
		case 2:
			lcdWriteNybble(LCD_NYBBLE_CMD | 0b00000011);
			break;

		case 3:
			lcdWriteNybble(LCD_NYBBLE_CMD | 0b00000010);
			break;

		case 4:
			lcdWriteCommand(
				LCD_CMD_FUNCTION |
				LCD_CMD_FUNCTION_TWOLINES |
				LCD_CMD_FUNCTION_FONT5X8);
			break;

		case 5:
			lcdWriteCommand(LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_OFF);
			break;

		case 6:
			lcdWriteCommand(LCD_CMD_DISPLAYCLEAR);
			break;

		case 7:
			lcdWriteCommand(
				LCD_CMD_ENTRYMODE |
				LCD_CMD_ENTRYMODE_INCREMENT |
				LCD_CMD_ENTRYMODE_NOSHIFT);
			break;

		case 8:
			lcdWriteCommand(LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_ON);
			break;

		default:
			lcdState.flags.isBusy = 0;
			eventPublish(LCD_ENABLED, &emptyEventArgs);
			waitForLcdCommand.handler = (NearScheduleHandler) 0;
	}

	nearSchedulerAdd(&waitForLcdCommand);
}
