#ifndef __CLUCK3SESAME_SRC_UI_UI_H
#define __CLUCK3SESAME_SRC_UI_UI_H
#include "../Platform/Event.h"
#include "../Ui.h"

#define UI_SCREEN_WIDTH 16
#define UI_SCREEN_HEIGHT 2
#define UI_CURSOR_AT(x, y) ((uint8_t) (((y) * (UI_SCREEN_WIDTH + 1)) + (x)))
#define UI_NO_CURSOR UI_CURSOR_AT(0, UI_SCREEN_HEIGHT)

struct ButtonBehaviour
{
	void (*onPressed)(void);
	void (*onReleased)(void);
};

struct ButtonsBehaviour
{
	const struct ButtonBehaviour *left;
	const struct ButtonBehaviour *right;
};

struct UiInput
{
	union
	{
		struct
		{
			char min;
			char max;
		} range;

		struct
		{
			uint8_t cursorPositions[3];
		} options;
	} menu;

	uint8_t selectedOptionIndex;
	uint8_t cursorPosition;
	const struct ButtonsBehaviour *buttons;
	void (*onPreEnter)(void);
	void (*onEnter)(void);
};

struct UiState
{
	union
	{
		uint16_t all;
		struct
		{
			unsigned int isInitialSetupRequired : 1;
			unsigned int isScreenOn : 1;
			unsigned int isLcdEnabled : 1;
			unsigned int isScreenTimeoutDisabled : 1;
			unsigned int isScreenBeingBlitted : 1;
			unsigned int isScreenBlitDirty : 1;
			unsigned int isLeftButtonPressed : 1;
			unsigned int isRightButtonPressed : 1;
			unsigned int isButtonPressTurningOnScreen : 1;
		} bits;
	} flags;

	char screen[2 * (UI_SCREEN_WIDTH + 1)];
	uint8_t screenTimeoutCount;

	struct UiInput input;
};

union NvmSettings;

extern struct UiState uiState;
extern union NvmSettings uiNvmSettings;

extern const struct ButtonsBehaviour uiInputIsUninitialised;
extern const struct ButtonsBehaviour uiInputIsRange;
extern const struct ButtonsBehaviour uiInputIsRangeOfTwo;
extern const struct ButtonsBehaviour uiInputIsOptions;

extern const struct ButtonBehaviour uiInputIgnore;
extern const struct ButtonBehaviour uiInputIncrementRange;
extern const struct ButtonBehaviour uiInputToggleRangeOfTwo;
extern const struct ButtonBehaviour uiInputMoveToNextOption;
extern const struct ButtonBehaviour uiInputEntered;

extern void uiOnSystemInitialised(const struct Event *event);

extern void uiInputOnButtonsPressed(const struct Event *event);
extern void uiInputOnButtonsReleased(const struct Event *event);

extern void uiScreenOn(void);
extern void uiScreenOff(void);
extern void uiScreenStartTimeout(void);
extern void uiScreenBlit(void);
extern int8_t uiScreenSignAndTwoDigitsFromPosition(uint8_t cursorPosition);
extern uint8_t uiScreenTwoDigitsFromPosition(uint8_t cursorPosition);

extern void uiDateAndTimeEntryScreen(void);
extern void uiDateAndTimeStatusScreen(void);
extern void uiLatitudeAndLongitudeEntryScreen(void);

extern void uiDoorCalibrationScreen(void);

#endif
