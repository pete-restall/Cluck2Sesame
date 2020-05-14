#ifndef __CLUCK3SESAME_SRC_UI_UI_H
#define __CLUCK3SESAME_SRC_UI_UI_H
#include "../Ui.h"

struct UiState
{
	union
	{
		uint8_t all;
		struct
		{
			unsigned int isScreenOn : 1;
			unsigned int isScreenTimeoutDisabled : 1;
			unsigned int isLeftButtonPressed : 1;
			unsigned int isRightButtonPressed : 1;
		} bits;
	} flags;
};

extern struct UiState uiState;

extern void uiOnButtonsPressed(const struct Event *event);
extern void uiOnButtonsReleased(const struct Event *event);

extern void uiScreenOn(void);
extern void uiScreenOff(void);
extern void uiScreenStartTimeout(void);

#endif
