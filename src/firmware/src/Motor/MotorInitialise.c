#include <xc.h>

#include "../Event.h"
#include "../NvmSettings.h"
#include "../VoltageRegulator.h"

#include "Motor.h"

#define CM1NCH_CURRENT_SENSE_PIN 0b011
#define CM1PCH_DAC 0b101

static void onVoltageRegulatorEnabled(const struct Event *event);
static void onVoltageRegulatorDisabled(const struct Event *event);

void motorInitialise(void)
{
	LATC &= 0b00110011;
	ANSELC &= 0b00110011;
	TRISC &= 0b00110011;

	LATBbits.LATB1 = 0;
	ANSELBbits.ANSB1 = 1;
	TRISBbits.TRISB1 = 0;

	static const struct EventSubscription onVoltageRegulatorEnabledSubscription =
	{
		.type = VOLTAGE_REGULATOR_ENABLED,
		.handler = &onVoltageRegulatorEnabled,
		.state = (void *) 0
	};

	eventSubscribe(&onVoltageRegulatorEnabledSubscription);

	static const struct EventSubscription onVoltageRegulatorDisabledSubscription =
	{
		.type = VOLTAGE_REGULATOR_DISABLED,
		.handler = &onVoltageRegulatorDisabled,
		.state = (void *) 0
	};

	eventSubscribe(&onVoltageRegulatorDisabledSubscription);
}

static void onVoltageRegulatorEnabled(const struct Event *event)
{
	TRISBbits.TRISB1 = 1;
	TRISCbits.TRISC2 = 1;
	TRISCbits.TRISC3 = 1;
	PMD2bits.DAC1MD = 0;
	PMD2bits.CMP1MD = 0;
	PMD1bits.TMR1MD = 0;
	PMD3bits.CCP1MD = 0;
	PMD3bits.PWM4MD = 0;
	PMD5bits.CLC2MD = 0;
	PMD4bits.CWG1MD = 0;

	DAC1CON1 = nvmSettings.motor.currentLimit;
	DAC1CON0 = _DAC1CON0_EN_MASK;

	PIR2bits.C1IF = 0;
	CM1NCH = CM1NCH_CURRENT_SENSE_PIN;
	CM1PCH = CM1PCH_DAC;
	CM1CON1 = _CM1CON1_INTP_MASK;
	CM1CON0 = _CM1CON0_EN_MASK | _CM1CON0_HYS_MASK;
	// FEED COMPARATOR TO CWG1 AUTO-SHUTDOWN
	// FEED CCP1 OUTPUT TO CWG1 AUTO-SHUTDOWN (VIA CLC2)
}

static void onVoltageRegulatorDisabled(const struct Event *event)
{
	PMD4bits.CWG1MD = 1;
	PMD5bits.CLC2MD = 1;
	PMD3bits.PWM4MD = 1;
	PMD3bits.CCP1MD = 1;
	PMD1bits.TMR1MD = 1;
	PMD2bits.CMP1MD = 1;
	PMD2bits.DAC1MD = 1;
	TRISBbits.TRISB1 = 0;
	TRISCbits.TRISC2 = 0;
	TRISCbits.TRISC3 = 0;
}
