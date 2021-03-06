	#include "Platform.inc"
	#include "FarCalls.inc"
	#include "Lcd.inc"
	#include "../../LcdStates.inc"
	#include "../../PollAfterLcdMock.inc"
	#include "../../../Motor/IsMotorVddEnabledStub.inc"
	#include "TestFixture.inc"

	radix decimal

MotorVddEnabledTest code
	global testArrange

testArrange:
	movlw 1
	fcall initialiseIsMotorVddEnabledStub
	fcall initialisePollAfterLcdMock
	fcall initialiseLcd

testAct:
	setLcdState LCD_STATE_ENABLE_WAITFORMOTORVDD
	fcall pollLcd

testAssert:
	.aliasForAssert lcdState, _a
	.aliasLiteralForAssert LCD_STATE_ENABLE_WAITFORMORETHAN40MS, _b
	.assert "_a == _b, 'Expected state to be LCD_STATE_ENABLE_WAITFORMORETHAN40MS.'"

	banksel calledPollAfterLcd
	.assert "calledPollAfterLcd != 0, 'Next poll in chain was not called.'"
	return

	end
