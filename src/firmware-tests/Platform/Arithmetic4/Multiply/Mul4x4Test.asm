	#include "Platform.inc"
	#include "FarCalls.inc"
	#include "Arithmetic4.inc"
	#include "TestFixture.inc"

	radix decimal

	udata
	global expectedRAA
	global expectedRBA
	global expectedW
	global expectedCarry
	global expectedZero
	global carry
	global zero

expectedRAA res 1
expectedRBA res 1
expectedW res 1

expectedCarry res 1
expectedZero res 1

capturedW res 1
capturedStatus res 1
carry res 1
zero res 1

Mul4x4Test code
	global testArrange

testArrange:
	banksel carry
	clrf carry
	clrf zero

testAct:
	fcall mul4x4

	banksel capturedW
	movwf capturedW

	swapf STATUS, W
	movwf capturedStatus
	swapf capturedStatus

	btfsc capturedStatus, C
	bsf carry, 0

	btfsc capturedStatus, Z
	bsf zero, 0

testAssert:
	.aliasForAssert RAA, _a
	.aliasForAssert expectedRAA, _b
	.assert "_a == _b, 'RAA expectation failure.'"

	.aliasForAssert RBA, _a
	.aliasForAssert expectedRBA, _b
	.assert "_a == _b, 'RBA expectation failure.'"

	banksel capturedW
	.assert "capturedW == expectedW, 'W expectation failure.'"
	.assert "carry == expectedCarry, 'Carry expectation failure.'"
	.assert "zero == expectedZero, 'Zero expectation failure.'"
	return

	end
