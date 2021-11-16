#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "../Event.h"
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

static void uiDateAndTimeScreenEnterNextDigit(void);
static void uiDateAndTimeScreenTimeChanged(const struct Event *event);
static void uiDateAndTimeScreenStatusExit(void);

static const struct EventSubscription onTimeChangedSubscription =
{
	.type = TIME_CHANGED,
	.handler = &uiDateAndTimeScreenTimeChanged
};

static const char *const uiDateAndTimeScreens =
	"Today is...     \0"
	"202Y-MM-DD hh:mm\0"
	"Temp, Volt, etc.";

#define DATE_AND_TIME_ENTRY_SCREEN_LINES (uiDateAndTimeScreens + 0)
#define DATE_AND_TIME_STATUS_SCREEN_LINES (uiDateAndTimeScreens + UI_SCREEN_WIDTH + 1)

void uiDateAndTimeEntryMenuScreen(void)
{
	uiState.menu.itemText = "Set clock ?     ";
	uiState.menu.onNext = &uiDoorCalibrationMenuScreen;
	uiState.menu.onOk = &uiDateAndTimeEntryScreen;
	uiMenuScreen();
}

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
	uiState.input.onEnter = &uiDateAndTimeScreenEnterNextDigit;
	uiState.input.onPreEnter = 0;
	uiScreenBlit();
}

static void uiDateAndTimeScreenEnterNextDigit(void)
{
	static struct DateAndTimeSet dateAndTime;

	uiState.input.menu.range.min = '0';
	switch (++uiState.input.cursorPosition)
	{
		case DATE_YY2_POS:
			uiState.input.menu.range.max = '9';
			break;

		case DATE_YY_MM_SEPARATOR_POS:
			uiState.input.cursorPosition++;
			dateAndTime.date.year = uiScreenTwoDigitsFromPosition(DATE_YY1_POS);
			uiState.input.menu.range.max = '1';
			break;

		case DATE_MM2_POS:
			if (uiState.screen[DATE_MM1_POS] == '1')
			{
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
			uiState.input.menu.range.max = '2';
			break;

		case TIME_HH2_POS:
			if (uiState.screen[TIME_HH1_POS] == '2')
				uiState.input.menu.range.max = '3';
			else
				uiState.input.menu.range.max = '9';
			break;

		case TIME_HH_MM_SEPARATOR_POS:
			uiState.input.cursorPosition++;
			dateAndTime.time.hour = uiScreenTwoDigitsFromPosition(TIME_HH1_POS);
			uiState.input.menu.range.max = '5';
			break;

		case TIME_MM2_POS:
			uiState.input.menu.range.max = '9';
			break;

		case DATE_ENTRY_COMPLETED:
			dateAndTime.time.minute = uiScreenTwoDigitsFromPosition(TIME_MM1_POS);
			// TODO: THIS NEEDS TO BE LOCAL TIME, NOT GMT...BUT IT'LL DO FOR NOW...
			clockSetNowGmt(&dateAndTime);
			if (uiState.flags.bits.isInitialSetupRequired)
			{
				uiLatitudeAndLongitudeEntryScreen();
			}
			else
			{
				UI_DEFAULT_SCREEN();
			}
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

	eventSubscribe(&onTimeChangedSubscription);
	uiDateAndTimeScreenTimeChanged((const struct Event *) 0);

	uiState.input.cursorPosition = UI_NO_CURSOR;
	uiState.input.buttons = &uiInputIsStatusScreen;
	uiState.input.onEnter = &uiDateAndTimeScreenStatusExit;
	uiScreenBlit();
}

static void uiDateAndTimeScreenTimeChanged(const struct Event *event)
{
	static struct DateAndTimeGet now;
	// TODO: THIS NEEDS TO BE LOCAL TIME, NOT GMT...BUT IT'LL DO FOR NOW...
	clockGetNowGmt(&now);

	uiScreenTwoDigitsToPosition(DATE_YY1_POS, now.date.year);
	uiScreenTwoDigitsToPosition(DATE_MM1_POS, now.date.month);
	uiScreenTwoDigitsToPosition(DATE_DD1_POS, now.date.day);

	uiScreenTwoDigitsToPosition(TIME_HH1_POS, now.time.hour);
	uiScreenTwoDigitsToPosition(TIME_MM1_POS, now.time.minute);
}

static void uiDateAndTimeScreenStatusExit(void)
{
	eventUnsubscribe(&onTimeChangedSubscription);
	uiDoorControlMenuScreen();
}
