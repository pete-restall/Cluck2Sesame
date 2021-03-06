	#ifndef __CLUCK2SESAME_PLATFORM_SMPS_SMPS_INC
	#define __CLUCK2SESAME_PLATFORM_SMPS_SMPS_INC
	#include "Platform.inc"

	radix decimal

	.module SMPS

	.externalVariable smpsFlags
	.externalVariable enableSmpsCount
	.externalVariable enableSmpsHighPowerModeCount
	.externalVariable smpsEnabledTimestamp

SMPS_PORT equ PORTC
SMPS_TRIS equ TRISC

SMPS_EN_PIN equ RC7
SMPS_EN_PIN_TRIS equ TRISC7
SMPS_EN_PIN_ANSH equ ANS9

PIC_VDD_PORT equ PORTB
PIC_VDD_TRIS equ TRISB

PIC_VDD_SMPS_EN_PIN equ RB7
PIC_VDD_SMPS_EN_PIN_TRIS equ TRISB7

SMPS_FLAG_VDDSTABLE equ 0
SMPS_FLAG_WAITFORSTABLEVDD equ 1
SMPS_FLAG_HIGHPOWERMODE equ 2

	#endif
