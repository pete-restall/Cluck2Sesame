#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "../Platform/Event.h"
#include "../Platform/NvmSettings.h"

#include "../Door.h"

#include "Ui.h"

#define MOVE_DOOR_MENU_LINE "Move door ?     "
#define MOVE_DOOR_LINE "Move door...    "

#define CALIBRATE_DOOR_MENU_LINE "Calibrate door ?"
#define CALIBRATE_DOOR_LINE "Move door to top"

#define DOOR_CONTROL_LINE ">UP   DN    DONE"
#define UP_POS UI_CURSOR_AT(0, 1)
#define DOWN_POS UI_CURSOR_AT(5, 1)
#define DONE_POS UI_CURSOR_AT(11, 1)

static void uiDoorControlScreen(void);
static void uiDoorControlScreenWireInputAndDisplay(void);
static void uiDoorControlScreenOptionSelected(void);
static void uiDoorControlScreenOptionAfterSelected(void);
static void uiDoorOnCalibrated(const struct Event *event);

static const struct EventSubscription onDoorCalibratedSubscription =
{
	.type = DOOR_CALIBRATED,
	.handler = &uiDoorOnCalibrated,
	.state = (void *) 0
};

void uiDoorControlMenuScreen(void)
{
	uiState.menu.itemText = MOVE_DOOR_MENU_LINE;
	uiState.menu.onNext = &uiDoorCalibrationMenuScreen;
	uiState.menu.onOk = &uiDoorControlScreen;
	uiMenuScreen();
}

static void uiDoorControlScreen(void)
{
	memcpy(
		&uiState.screen[0],
		MOVE_DOOR_LINE,
		UI_SCREEN_WIDTH + 1);

	memcpy(
		&uiState.screen[UI_SCREEN_WIDTH + 1],
		DOOR_CONTROL_LINE,
		UI_SCREEN_WIDTH + 1);

	uiState.flags.bits.isDoorBeingCalibrated = 0;
	uiDoorControlScreenWireInputAndDisplay();
}

static void uiDoorControlScreenWireInputAndDisplay(void)
{
	uiState.input.menu.options.cursorPositions[0] = UP_POS;
	uiState.input.menu.options.cursorPositions[1] = DOWN_POS;
	uiState.input.menu.options.cursorPositions[2] = DONE_POS;
	uiState.input.cursorPosition = UP_POS;
	uiState.input.selectedOptionIndex = 0;
	uiState.input.buttons = &uiInputIsOptions;
	uiState.input.onPreEnter = &uiDoorControlScreenOptionSelected;
	uiState.input.onEnter = &uiDoorControlScreenOptionAfterSelected;

	uiScreenBlit();
}

static void uiDoorControlScreenOptionSelected(void)
{
	// TODO: COULD DO WITH CAPTURING DOOR_ABORTED AND THEN SHOWING AN ALERT SCREEN WITH A MESSAGE FOR A FAULT ('JAMMED/TOO HEAVY', 'SPOOL REVERSED', etc.)  PROBABLY A BACKLIGHT FLASH AS WELL.  ALERT SCREEN WILL BE GENERALLY USEFUL, SO MAKE A 'uiAlertScreen.c' MODULE FOR IT.
	if (uiState.input.selectedOptionIndex == 0)
	{
		doorManualStartOpening();
		uiState.flags.bits.isDoorBeingManuallyControlled = 1;
	}

	if (uiState.input.selectedOptionIndex == 1)
	{
		doorManualStartClosing();
		uiState.flags.bits.isDoorBeingManuallyControlled = 1;
	}
}

static void uiDoorControlScreenOptionAfterSelected(void)
{
	if (uiState.flags.bits.isDoorBeingManuallyControlled)
	{
		doorManualStop();
		uiState.flags.bits.isDoorBeingManuallyControlled = 0;
	}
	else if (uiState.input.selectedOptionIndex == 2)
	{
		if (uiState.flags.bits.isInitialSetupRequired)
		{
			uiNvmSettings.application.door.mode.isManuallyOverridden = 0;
			uiNvmSettings.application.door.mode.isSunEventDriven = 1;
			uiNvmSettings.application.door.mode.isTimeDriven = 0;
		}

		if (uiState.flags.bits.isDoorBeingCalibrated)
		{
			uiState.flags.bits.isDoorBeingCalibrated = 0;

			eventSubscribe(&onDoorCalibratedSubscription);
			// TODO: THIS NEEDS TO BE A FUNCTION INSTEAD
			memcpy(
				uiState.screen,
				"! DO NOT TOUCH !\0"
				"Calibrating...  ",
				sizeof(uiState.screen));

			uiState.flags.bits.isScreenTimeoutDisabled = 1;
			uiState.input.buttons = &uiInputIsUninitialised;
			uiState.input.cursorPosition = UI_NO_CURSOR;
			uiScreenBlit();
			doorCalibrate();
		}
		else
			uiDateAndTimeStatusScreen();
	}
}

void uiDoorCalibrationMenuScreen(void)
{
	uiState.menu.itemText = CALIBRATE_DOOR_MENU_LINE;
	uiState.menu.onNext = &UI_LAST_SETTINGS_SCREEN;
	uiState.menu.onOk = &uiDoorCalibrationScreen;
	uiMenuScreen();
}

void uiDoorCalibrationScreen(void)
{
	memcpy(
		&uiState.screen[0],
		CALIBRATE_DOOR_LINE,
		UI_SCREEN_WIDTH + 1);

	memcpy(
		&uiState.screen[UI_SCREEN_WIDTH + 1],
		DOOR_CONTROL_LINE,
		UI_SCREEN_WIDTH + 1);

	uiState.flags.bits.isDoorBeingCalibrated = 1;
	uiDoorControlScreenWireInputAndDisplay();
}

static void uiDoorOnCalibrated(const struct Event *event)
{
	uiScreenStartTimeout();
	eventUnsubscribe(&onDoorCalibratedSubscription);

	// TODO: IF FAULT THEN... (AND GO BACK TO THE CALIBRATION SCREEN...)

	const struct DoorCalibrated *calibrated = (const struct DoorCalibrated *) event->args;
	uiNvmSettings.application.door.height = calibrated->height;

	if (uiState.flags.bits.isInitialSetupRequired)
	{
		uiOnSystemInitialSetupCompleted();
	}
	else
	{
		UI_DEFAULT_SCREEN();
	}
}
