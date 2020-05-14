#ifndef __CLUCK3SESAME_SRC_UI_UI_H
#define __CLUCK3SESAME_SRC_UI_UI_H
#include "../Platform/Event.h"
#include "../Ui.h"

#define UI_SCREEN_WIDTH 16

enum UiInputStyle
{
	ScreenSelection,
	CharacterRange,
	Options,
	Motor
};

struct UiState
{
	union
	{
		uint8_t all;
		struct
		{
			unsigned int isInitialSetupRequired : 1;
			unsigned int isScreenOn : 1;
			unsigned int isScreenTimeoutDisabled : 1;
			unsigned int isLeftButtonPressed : 1;
			unsigned int isRightButtonPressed : 1;
		} bits;
	} flags;

	char screen[2][UI_SCREEN_WIDTH + 1];

	//////////////////////////////////////////////////////
	///////// TODO: SOMETHING LIKE THIS, PERHAPS ?
	struct
	{
		union
		{
			struct
			{
				uint8_t cursorPosition;
				uint8_t min;
				uint8_t max;
			} range;

			struct
			{
				uint8_t cursorPositions[3];
			} options;
		} input;
		enum UiInputStyle inputStyle;
		uint8_t selectedIndex;
	} menu;
	//////////////////////////////////////////////////////
};

extern struct UiState uiState;

extern void uiOnButtonsPressed(const struct Event *event);
extern void uiOnButtonsReleased(const struct Event *event);
extern void uiOnSystemInitialised(const struct Event *event);

extern void uiScreenOn(void);
extern void uiScreenOff(void);
extern void uiScreenStartTimeout(void);
extern void uiScreenBlit(void);

extern void uiEnterInitialDateAndTime(void);

#endif
