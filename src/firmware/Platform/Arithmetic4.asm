	#include "Platform.inc"
	#include "GeneralPurposeRegisters.inc"

	radix decimal

Arithmetic4 code
	global mul4x4

mul4x4:
	.safelySetBankFor RAA
	clrw
	bcf STATUS, C
	btfsc RAA, 0
	addwf RBA, W
	rlf RBA
	btfsc RAA, 1
	addwf RBA, W
	rlf RBA
	btfsc RAA, 2
	addwf RBA, W
	rlf RBA
	btfsc RAA, 3
	addwf RBA, W
	rlf RBA
	swapf RBA
	return

	end
