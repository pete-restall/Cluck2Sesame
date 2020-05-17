#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "../Platform/NvmSettings.h"

#include "../Location.h"

#include "Ui.h"

#if LONGLAT_ONE_DEGREE != 10
#error The UI is based on the assumption that one degree is 10 units and the fractional part is a single unit
#endif

#define LATITUDE_ORIGIN_DEGREES 55
#define LONGITUDE_ORIGIN_DEGREES 0

#define LAT_SIGN_POS UI_CURSOR_AT(0, 1)
#define LAT_WHOLE1_POS (LAT_SIGN_POS + 1)
#define LAT_WHOLE2_POS (LAT_WHOLE1_POS + 1)
#define LAT_WHOLE_FRAC_SEPARATOR_POS (LAT_WHOLE2_POS + 1)
#define LAT_FRAC1_POS (LAT_WHOLE_FRAC_SEPARATOR_POS + 1)
#define LAT_LONG_SEPARATOR_POS (LAT_FRAC1_POS + 1)

#define LONG_SIGN_POS (LAT_LONG_SEPARATOR_POS + 1)
#define LONG_WHOLE1_POS (LONG_SIGN_POS + 1)
#define LONG_WHOLE2_POS (LONG_WHOLE1_POS + 1)
#define LONG_WHOLE_FRAC_SEPARATOR_POS (LONG_WHOLE2_POS + 1)
#define LONG_FRAC1_POS (LONG_WHOLE_FRAC_SEPARATOR_POS + 1)

#define LATLONG_ENTRY_COMPLETED (LONG_FRAC1_POS + 1)

static void uiLatitudeAndLongitudeScreenEnterNextDigit(void);
static int8_t uiDegreesFromScreenPosition(uint8_t cursorPosition, int8_t origin);

// TODO: THESE NEED WRITING AND PUTTING SOMEWHERE... 
static void uiLatitudeAndLongitudeStatusScreen(void) { uiState.input.buttons = &uiInputIsUninitialised; }
static void uiDoorCalibrationScreen(void) { uiState.screen uiState.input.buttons = &uiInputIsUninitialised; }
// END OF TODO...

void uiLatitudeAndLongitudeEntryScreen(void)
{
	memcpy(
		uiState.screen,
		"Lat-Long is...  \0"
		"+5y.y +xx.x     ",
		sizeof(uiState.screen));

	uiState.input.cursorPosition = LAT_WHOLE1_POS;
	uiState.input.menu.range.min = '5';
	uiState.input.menu.range.max = '6';
	uiState.input.buttons = &uiInputIsRange;
	uiState.input.onEnter = &uiLatitudeAndLongitudeScreenEnterNextDigit;
	uiScreenBlit();
}

static void uiLatitudeAndLongitudeScreenEnterNextDigit(void)
{
	switch (++uiState.input.cursorPosition)
	{
		case LAT_WHOLE2_POS:
			uiState.input.menu.range.min = '0';
			if (uiState.screen[LAT_WHOLE1_POS] == '6')
				uiState.input.menu.range.max = '0';
			else
				uiState.input.menu.range.max = '9';
			break;

		case LAT_WHOLE_FRAC_SEPARATOR_POS:
			uiState.input.cursorPosition++;
			uiState.input.menu.range.min = '0';
			if (uiState.screen[LAT_WHOLE1_POS] == '6')
				uiState.input.menu.range.max = '0';
			else
				uiState.input.menu.range.max = '9';
			break;

		case LAT_LONG_SEPARATOR_POS:
			uiState.input.cursorPosition++;
			uiNvmSettings.application.location.latitudeOffset = uiDegreesFromScreenPosition(LAT_SIGN_POS, LATITUDE_ORIGIN_DEGREES);

			uiState.input.menu.range.min = '+';
			uiState.input.menu.range.max = '-';
			uiState.input.buttons = &uiInputIsRangeOfTwo;
			break;

		case LONG_WHOLE1_POS:
			uiState.input.menu.range.min = '0';
			uiState.input.menu.range.max = '1';
			uiState.input.buttons = &uiInputIsRange;
			break;

		case LONG_WHOLE2_POS:
			uiState.input.menu.range.min = '0';
			if (uiState.screen[LONG_WHOLE1_POS] == '1')
				uiState.input.menu.range.max = '0';
			else
				uiState.input.menu.range.max = '9';
			break;

		case LONG_WHOLE_FRAC_SEPARATOR_POS:
			uiState.input.cursorPosition++;
			uiState.input.menu.range.min = '0';
			if (uiState.screen[LONG_WHOLE1_POS] == '1')
				uiState.input.menu.range.max = '0';
			else
				uiState.input.menu.range.max = '9';
			break;

		case LATLONG_ENTRY_COMPLETED:
			uiNvmSettings.application.location.longitudeOffset = uiDegreesFromScreenPosition(LONG_SIGN_POS, LONGITUDE_ORIGIN_DEGREES);
			if (uiState.flags.bits.isInitialSetupRequired)
			{
				uiDoorCalibrationScreen();
			}
			else
			{
				nvmSettingsStore(&uiNvmSettings);
				uiLatitudeAndLongitudeStatusScreen();
			}
			return;
	}

	uiState.screen[uiState.input.cursorPosition] = uiState.input.menu.range.min;
	uiScreenBlit();
}

static int8_t uiDegreesFromScreenPosition(uint8_t cursorPosition, int8_t origin)
{
	int8_t offset = LONGLAT_ONE_DEGREE * (uiScreenSignAndTwoDigitsFromPosition(cursorPosition) - origin);
	uint8_t fractionalDigit = uiState.screen[cursorPosition + (LAT_FRAC1_POS - LAT_SIGN_POS)] - '0';
	if (offset < 0)
		return offset - fractionalDigit;

	return offset + fractionalDigit;
}
