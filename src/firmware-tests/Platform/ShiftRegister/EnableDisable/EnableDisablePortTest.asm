	#include "Platform.inc"
	#include "FarCalls.inc"
	#include "ShiftRegister.inc"
	#include "TestFixture.inc"

	radix decimal

	udata
	global numberOfEnableCalls
	global numberOfDisableCalls
	global forcePortcBeforeEnable
	global forcePortcBeforeDisable
	global expectedPortc

numberOfEnableCalls res 1
numberOfDisableCalls res 1
forcePortcBeforeEnable res 1
forcePortcBeforeDisable res 1
expectedPortc res 1

EnableDisablePortTest code
	global testArrange

testArrange:
	banksel ANSEL
	clrf ANSEL

	banksel ANSELH
	clrf ANSELH

	fcall initialiseShiftRegister

	banksel forcePortcBeforeEnable
	movf forcePortcBeforeEnable, W
	banksel PORTC
	movwf PORTC

testAct:
	banksel numberOfEnableCalls
	movf numberOfEnableCalls
	btfsc STATUS, Z
	goto callDisableShiftRegister

callEnableShiftRegister:
	fcall enableShiftRegister
	banksel numberOfEnableCalls
	decfsz numberOfEnableCalls
	goto callEnableShiftRegister

callDisableShiftRegister:
	banksel numberOfDisableCalls
	movf numberOfDisableCalls
	btfsc STATUS, Z
	goto testAssert

	banksel forcePortcBeforeDisable
	movf forcePortcBeforeDisable, W
	banksel PORTC
	movwf PORTC

callDisableShiftRegisterInLoop:
	fcall disableShiftRegister
	banksel numberOfDisableCalls
	decfsz numberOfDisableCalls
	goto callDisableShiftRegisterInLoop

testAssert:
	.aliasForAssert PORTC, _a
	.aliasForAssert expectedPortc, _b
	.assert "_a == _b, 'PORTC expectation failure.'"
	return

	end
