#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "../Platform/Clock.h"

#include "Ui.h"

#define DATE_YY1_POS UI_CURSOR_AT(2, 1)
#define DATE_YY2_POS (DATE_YY1_POS + 1)
#define DATE_YY_MM_SEPARATOR_POS (DATE_YY2_POS + 1)
#define DATE_MM1_POS (DATE_YY_MM_SEPARATOR_POS + 1)
#define DATE_MM2_POS (DATE_MM1_POS + 1)
#define DATE_MM_DD_SEPARATOR_POS (DATE_MM2_POS + 1)
#define DATE_DD1_POS (DATE_MM_DD_SEPARATOR_POS + 1)
#define DATE_DD2_POS (DATE_DD1_POS + 1)
#define DATE_TIME_SEPARATOR_POS (DATE_DD2_POS + 1)
#define TIME_HH1_POS (DATE_TIME_SEPARATOR_POS + 1)
#define TIME_HH2_POS (TIME_HH1_POS + 1)
#define TIME_HH_MM_SEPARATOR_POS (TIME_HH2_POS + 1)
#define TIME_MM1_POS (TIME_HH_MM_SEPARATOR_POS + 1)
#define TIME_MM2_POS (TIME_MM1_POS + 1)
#define DATE_ENTRY_COMPLETED (TIME_MM2_POS + 1)

static void uiEnterInitialDateAndTimeNextDigit(void);

static const char *const uiDateAndTimeScreens =
	"Today is...     \0"
	"202Y-MM-DD hh:mm\0"
	"Temp, Volt, etc.";

#define DATE_AND_TIME_ENTRY_SCREEN_LINES (uiDateAndTimeScreens + 0)
#define DATE_AND_TIME_STATUS_SCREEN_LINES (uiDateAndTimeScreens + UI_SCREEN_WIDTH)

void uiDateAndTimeEntryScreen(void)
{
	memcpy(
		uiState.screen,
		DATE_AND_TIME_ENTRY_SCREEN_LINES,
		sizeof(uiState.screen));

	uiState.input.cursorPosition = DATE_YY1_POS;
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
		case DATE_YY2_POS:
			uiState.input.menu.range.min = '0';
			uiState.input.menu.range.max = '9';
			break;

		case DATE_YY_MM_SEPARATOR_POS:
			uiState.input.cursorPosition++;
			dateAndTime.date.year = uiScreenTwoDigitsFromPosition(DATE_YY1_POS);
			uiState.input.menu.range.min = '0';
			uiState.input.menu.range.max = '1';
			break;

		case DATE_MM2_POS:
			if (uiState.screen[DATE_MM1_POS] == '1')
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

		case DATE_MM_DD_SEPARATOR_POS:
			uiState.input.cursorPosition++;
			dateAndTime.date.month = uiScreenTwoDigitsFromPosition(DATE_MM1_POS);
			uiState.input.menu.range.min = '0';
			if (dateAndTime.date.month == 2)
				uiState.input.menu.range.max = '2';
			else
				uiState.input.menu.range.max = '3';
			break;

		case DATE_DD2_POS:
			if (uiState.screen[DATE_DD1_POS] == '0')
			{
				uiState.input.menu.range.min = '1';
				uiState.input.menu.range.max = '9';
			}
			else if (uiState.screen[DATE_DD1_POS] == uiState.input.menu.range.max)
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

		case DATE_TIME_SEPARATOR_POS:
			uiState.input.cursorPosition++;
			dateAndTime.date.day = uiScreenTwoDigitsFromPosition(DATE_DD1_POS);
			uiState.input.menu.range.min = '0';
			uiState.input.menu.range.max = '2';
			break;

		case TIME_HH2_POS:
			uiState.input.menu.range.min = '0';
			if (uiState.screen[TIME_HH1_POS] == '2')
				uiState.input.menu.range.max = '3';
			else
				uiState.input.menu.range.max = '9';
			break;

		case TIME_HH_MM_SEPARATOR_POS:
			uiState.input.cursorPosition++;
			dateAndTime.time.hour = uiScreenTwoDigitsFromPosition(TIME_HH1_POS);
			uiState.input.menu.range.min = '0';
			uiState.input.menu.range.max = '5';
			break;

		case TIME_MM2_POS:
			uiState.input.menu.range.min = '0';
			uiState.input.menu.range.max = '9';
			break;

		case DATE_ENTRY_COMPLETED:
			dateAndTime.time.minute = uiScreenTwoDigitsFromPosition(TIME_MM1_POS);
			// TODO: THIS NEEDS TO BE LOCAL TIME, NOT GMT...BUT IT'LL DO FOR NOW...
			clockSetNowGmt(&dateAndTime);
			if (uiState.flags.bits.isInitialSetupRequired)
				uiLatitudeAndLongitudeEntryScreen();
			else
				uiDateAndTimeStatusScreen();
			return;
	}

	uiState.screen[uiState.input.cursorPosition] = uiState.input.menu.range.min;
	uiScreenBlit();
}

void uiDateAndTimeStatusScreen(void)
{
	memcpy(
		uiState.screen,
		DATE_AND_TIME_STATUS_SCREEN_LINES,
		sizeof(uiState.screen));

	// TODO...
	uiState.input.cursorPosition = UI_NO_CURSOR;
	uiState.input.buttons = &uiInputIsUninitialised;
	uiScreenBlit();
}
