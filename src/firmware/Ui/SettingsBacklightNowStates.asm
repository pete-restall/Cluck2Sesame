	#include "Platform.inc"
	#include "FarCalls.inc"
	#include "Flash.inc"
	#include "Lcd.inc"
	#include "States.inc"
	#include "OptionStates.inc"
	#include "WaitButtonPressState.inc"

	radix decimal

ON_CURSOR_POSITION equ LCD_SECOND_LINE | 0
OFF_CURSOR_POSITION equ LCD_SECOND_LINE | 11

	defineUiState UI_STATE_SETTINGS_BACKLIGHTNOW
		fcall isLcdIdle
		xorlw 0
		btfsc STATUS, Z
		returnFromUiState

		loadFlashAddressOf screen
		fcall putScreenFromFlash

		setButtonPressEventStates UI_STATE_SETTINGS_BACKLIGHTNOW_LEFT, UI_STATE_SETTINGS_BACKLIGHTNOW_RIGHT, UI_STATE_SETTINGS_BACKLIGHTNOW_ENTER

		movlw ON_CURSOR_POSITION
		movwf uiOption1Position

		movlw OFF_CURSOR_POSITION
		movwf uiOption2Position
		movwf uiSelectedOptionPosition ; TODO: BUG !  ONLY VALID IF FROM BOOT - IF LIGHT IS ON WHEN THIS IS ENTERED THEN THE OPTION SHOULD BE UPDATED...

		movlw UI_OPTION_POSITIONUNUSED
		movwf uiOption3Position

		setUiState UI_STATE_OPTION_CHANGED
		returnFromUiState


	defineUiStateInSameSection UI_STATE_SETTINGS_BACKLIGHTNOW_LEFT
		setUiState UI_STATE_OPTION_CHANGED
		movlw ON_CURSOR_POSITION
		movwf uiSelectedOptionPosition
		fcall setLcdBacklightFlag
		returnFromUiState


	defineUiStateInSameSection UI_STATE_SETTINGS_BACKLIGHTNOW_RIGHT
		setUiState UI_STATE_OPTION_CHANGED
		movlw OFF_CURSOR_POSITION
		movwf uiSelectedOptionPosition
		fcall clearLcdBacklightFlag
		returnFromUiState


	defineUiStateInSameSection UI_STATE_SETTINGS_BACKLIGHTNOW_ENTER
		setUiState UI_STATE_SETTINGS_BACKLIGHTINTHEDARK
		returnFromUiState

screen:
	da "Backlight (Now) "
	da " ON         OFF "

	end
