	#ifndef __CLUCK2SESAME_PLATFORM_POWERMANAGEMENT_POWERMANAGEMENT_INC
	#define __CLUCK2SESAME_PLATFORM_POWERMANAGEMENT_POWERMANAGEMENT_INC
	#include "Platform.inc"

	radix decimal

	.module POWERMANAGEMENT

POWER_FLAG_PREVENTSLEEP equ 0
POWER_FLAG_SLEEPING equ 1

	#ifndef __CLUCK2SESAME_PLATFORM_POWERMANAGEMENT_INITIALISE_ASM
	.externalVariable powerManagementFlags
	.externalVariable fastClockCount
	#endif

	#endif
