	#include "Platform.inc"
	#include "FarCalls.inc"
	#include "MclrReset.inc"
	#include "TestFixture.inc"

	radix decimal

UlpwuDisabledTest code
	global testArrange

testArrange:
	banksel PCON
	movlw 0xff 
	movwf PCON

testAct:
	fcall initialiseAfterMclrReset

testAssert:
	banksel PCON
	.assert "(pcon & 0x20) == 0, 'ULPWUE bit should not be set.'"
	return

	end
