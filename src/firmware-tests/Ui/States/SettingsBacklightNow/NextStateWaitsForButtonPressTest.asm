	#include "Mcu.inc"
	#include "FarCalls.inc"
	#include "Ui.inc"
	#include "TestFixture.inc"

	radix decimal

NextStateWaitsForButtonPressTest code
	global testAct

testAct:
	fcall pollUi
	return

	end
