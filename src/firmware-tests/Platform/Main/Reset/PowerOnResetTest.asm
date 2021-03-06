	#include "Platform.inc"
	#include "FarCalls.inc"
	#include "TestFixture.inc"
	#include "../ResetMocks.inc"

	radix decimal

	extern main

PowerOnResetTest code
	global testArrange
	global testAssert

testArrange:
	fcall initialiseResetMocks

testAct:
	fcall main

testAssert:
	banksel calledInitialiseAfterPowerOnReset
	.assert "calledInitialiseAfterPowerOnReset != 0, 'POR condition did not call initialiseAfterPowerOnReset.'"
	.assert "calledInitialiseAfterBrownOutReset == 0, 'POR condition called initialiseAfterBrownOutReset.'"
	.assert "calledInitialiseAfterMclrReset == 0, 'POR condition called initialiseAfterMclrReset.'"
	.done

	end
