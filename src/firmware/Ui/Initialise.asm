	#include "Platform.inc"
	#include "TailCalls.inc"
	#include "InitialisationChain.inc"
	#include "Ui.inc"
	#include "States.inc"

	radix decimal

	extern INITIALISE_AFTER_UI

Ui code
	global initialiseUi

initialiseUi:
	.safelySetBankFor uiFlags
	clrf uiFlags
	clrf uiState

	tcall INITIALISE_AFTER_UI

	end
