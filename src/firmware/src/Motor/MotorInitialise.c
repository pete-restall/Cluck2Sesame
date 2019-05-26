#include <xc.h>

#include "../Event.h"
#include "../NvmSettings.h"
#include "../VoltageRegulator.h"

#include "Motor.h"

#define CWG1CON0_SYNCHRONOUS_STEERING_MODE (0b001 << _CWG1CON0_MODE_POSITION)
#define CWG1AS0_FORCE_ALL_ZERO_ON_SHUTDOWN ( \
	(0b10 << _CWG1AS0_LSBD_POSITION) | \
	(0b10 << _CWG1AS0_LSAC_POSITION))

#define CWG1AS1_SHUTDOWN_ON_COMPARATOR1 _CWG1AS1_AS2E_MASK
#define CWG1AS1_SHUTDOWN_ON_CLC2 _CWG1AS1_AS4E_MASK
#define CWG1CLKCON_USE_FOSC 1
#define CWG1DAT_USE_PWM4 0b0100

#define PPS_OUT_CWG1A 0x05
#define PPS_OUT_CWG1B 0x06

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
	PMD2bits.DAC1MD = 0;
	DAC1CON1 = nvmSettings.motor.currentLimit;
	DAC1CON0 = _DAC1CON0_EN_MASK;

	TRISBbits.TRISB1 = 1;
	TRISCbits.TRISC2 = 1;
	TRISCbits.TRISC3 = 1;
	PMD2bits.CMP1MD = 0;
	PMD1bits.TMR1MD = 0;
	PMD3bits.CCP1MD = 0;
	PMD3bits.PWM4MD = 0;
	PMD5bits.CLC2MD = 0;
	PMD4bits.CWG1MD = 0;

// TODO: CLC2
// TODO: CCP1
// TODO: TMR1

	PWM4DCH = 0;
	PWM4DCL = 0;

	CWG1AS0 = CWG1AS0_FORCE_ALL_ZERO_ON_SHUTDOWN;
	CWG1AS1 = CWG1AS1_SHUTDOWN_ON_COMPARATOR1 | CWG1AS1_SHUTDOWN_ON_CLC2;
	CWG1STR = 0;
	CWG1CLKCON = CWG1CLKCON_USE_FOSC;
	CWG1DAT = CWG1DAT_USE_PWM4;

	CM1NCH = CM1NCH_CURRENT_SENSE_PIN;
	CM1PCH = CM1PCH_DAC;
	CM1CON1 = _CM1CON1_INTP_MASK;
	PIR2bits.C1IF = 0;

	PWM4CON = _PWM4CON_PWM4EN_MASK;
	CM1CON0 = _CM1CON0_EN_MASK | _CM1CON0_HYS_MASK;
	CWG1CON0 = CWG1CON0_SYNCHRONOUS_STEERING_MODE | _CWG1CON0_EN_MASK;

	RC6PPS = PPS_OUT_CWG1A;
	RC7PPS = PPS_OUT_CWG1B;
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
