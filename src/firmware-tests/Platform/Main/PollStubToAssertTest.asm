	#include "Platform.inc"
	#include "TailCalls.inc"

	radix decimal

	extern testAssert

PollStubToAssertTest code
	global pollForWork

pollForWork:
	tcall testAssert

	end
