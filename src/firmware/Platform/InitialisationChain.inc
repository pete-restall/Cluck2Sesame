	#ifndef __CLUCK2SESAME_PLATFORM_INITIALISATIONCHAIN_INC
	#define __CLUCK2SESAME_PLATFORM_INITIALISATIONCHAIN_INC

	radix decimal

	#define INITIALISE_FIRST initialiseClock
	#define INITIALISE_AFTER_CLOCK initialiseTimer0
	#define INITIALISE_AFTER_TIMER0 initialisePowerManagement
	#define INITIALISE_AFTER_POWERMANAGEMENT initialiseAdc
	#define INITIALISE_AFTER_ADC initialiseSmps
	#define INITIALISE_AFTER_SMPS initialiseMotor
	#define INITIALISE_AFTER_MOTOR initialiseShiftRegister
	#define INITIALISE_AFTER_SHIFTREGISTER initialiseLcd
	#define INITIALISE_AFTER_LCD initialiseIsr
	#define INITIALISE_AFTER_ISR initialiseButtons
	#define INITIALISE_AFTER_BUTTONS initialiseUi
	#define INITIALISE_AFTER_UI initialiseSunriseSunset
	#define INITIALISE_AFTER_SUNRISESUNSET initialiseDoor
	#define INITIALISE_AFTER_DOOR initialisationCompleted

	#endif
