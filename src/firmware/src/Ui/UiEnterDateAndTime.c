#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "../Platform/Clock.h"

#include "Ui.h"

static void uiEnterInitialDateAndTimeNextDigit(void);
static uint8_t uiTwoDigitsToDecimalFromScreenAt(uint8_t cursorPosition);

void uiEnterInitialDateAndTime(void)
{
	memcpy(
		uiState.screen,
		"Today is...     \0"
		"202Y-MM-DD hh:mm",
		sizeof(uiState.screen));

	uiState.input.cursorPosition = UI_CURSOR_AT(2, 1);
	uiState.input.menu.range.min = '2';
	uiState.input.menu.range.max = '9';
	uiState.input.buttons = &uiInputIsRange;
	uiState.input.onEnter = &uiEnterInitialDateAndTimeNextDigit;
	uiScreenBlit();
}

static void uiEnterInitialDateAndTimeNextDigit(void)
{
	static struct DateAndTimeSet dateAndTime;

	switch (++uiState.input.cursorPosition)
	{
		case UI_CURSOR_AT(3, 1):
			uiState.input.menu.range.min = '0';
			uiState.input.menu.range.max = '9';
			break;

		case UI_CURSOR_AT(4, 1):
			uiState.input.cursorPosition++;
			dateAndTime.date.year = uiTwoDigitsToDecimalFromScreenAt(UI_CURSOR_AT(2, 1));
			uiState.input.menu.range.min = '0';
			uiState.input.menu.range.max = '1';
			break;

		case UI_CURSOR_AT(6, 1):
			if (uiState.screen[UI_CURSOR_AT(5, 1)] == '1')
			{
				uiState.input.menu.range.min = '0';
				uiState.input.menu.range.max = '2';
			}
			else
			{
				uiState.input.menu.range.min = '1';
				uiState.input.menu.range.max = '9';
			}
			break;

		case UI_CURSOR_AT(7, 1):
			uiState.input.cursorPosition++;
			dateAndTime.date.month = uiTwoDigitsToDecimalFromScreenAt(UI_CURSOR_AT(5, 1));
			uiState.input.menu.range.min = '0';
			if (uiState.screen[UI_CURSOR_AT(5, 1)] == '0' && uiState.screen[UI_CURSOR_AT(6, 1)] == '2')
				uiState.input.menu.range.max = '2';
			else
				uiState.input.menu.range.max = '3';
			break;

		case UI_CURSOR_AT(9, 1):
			if (uiState.screen[UI_CURSOR_AT(8, 1)] == '0')
			{
				uiState.input.menu.range.min = '1';
				uiState.input.menu.range.max = '9';
			}
			else if (uiState.screen[UI_CURSOR_AT(8, 1)] == '3' || (uiState.screen[UI_CURSOR_AT(8, 1)] == '2' && dateAndTime.date.month == 2))
			{
				uiState.input.menu.range.min = '0';

				uint8_t daysInMonth = clockDaysInMonth(dateAndTime.date.month, dateAndTime.date.year);
				if (daysInMonth == 28)
					uiState.input.menu.range.max = '8';
				else if (daysInMonth == 29)
					uiState.input.menu.range.max = '9';
				else if (daysInMonth == 30)
					uiState.input.menu.range.max = '0';
				else
					uiState.input.menu.range.max = '1';
			}
			break;

		// TODO: NEED TO SET THE DAY PART OF 'dateAndTime' AND THEN DO THE TIME...
	}

	uiState.screen[uiState.input.cursorPosition] = uiState.input.menu.range.min;
	uiScreenBlit();
}

static uint8_t uiTwoDigitsToDecimalFromScreenAt(uint8_t cursorPosition)
{
	return
		(uiState.screen[cursorPosition] - '0') * 10 +
		(uiState.screen[cursorPosition + 1] - '0');
}
