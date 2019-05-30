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

#define CCP1CON_HOLD_IN_RESET_MODE (0b0000 << _CCP1CON_MODE_POSITION)

#define CM1NCH_CURRENT_SENSE_PIN 0b011
#define CM1PCH_DAC 0b101

#define T1CLK_USE_T1CKIN_PIN 0

#define CLCCON_MODE_AND4 (0b010 << _CLC2CON_LC2MODE_POSITION)
#define CLCSEL_CURRENT_SENSE_COMPARATOR 0b010110
#define CLCSEL_CCP 0b001111

#define PPS_IN_RC3 0x13

#define PPS_OUT_CWG1A 0x05
#define PPS_OUT_CWG1B 0x06

static void onVoltageRegulatorEnabled(const struct Event *event);
static inline void configureTimer1AndCcpForEncoder(void);
static inline void configureClcAsOrGateForCcpAndComparator(void);
static inline void configurePwmAsOffAndComparatorAsCurrentSenseUsingDac(void);
static inline void configureCwgForPwmOutputWithClcShutdown(void);
static void onVoltageRegulatorDisabled(const struct Event *event);

struct MotorState motorState;

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

	motorState.enableCount = 0;
	motorState.flags.all = 0;
}

static void onVoltageRegulatorEnabled(const struct Event *event)
{
	PMD2bits.DAC1MD = 0;
	DAC1CON1 = nvmSettings.motor.currentLimit;
	DAC1CON0 = _DAC1CON0_EN_MASK;

	TRISBbits.TRISB1 = 1;
	TRISCbits.TRISC2 = 1;
	TRISCbits.TRISC3 = 1;

	configureTimer1AndCcpForEncoder();
	configureClcAsOrGateForCcpAndComparator();
	configureCwgForPwmOutputWithClcShutdown();
	configurePwmAsOffAndComparatorAsCurrentSenseUsingDac();

	RC6PPS = PPS_OUT_CWG1A;
	RC7PPS = PPS_OUT_CWG1B;

	if (motorState.enableCount != 0)
	{
		static const struct MotorEnabled eventArgs = { };
		eventPublish(MOTOR_ENABLED, &eventArgs);
		motorState.flags.isFullyEnabled = 1;
	}
}

static inline void configureTimer1AndCcpForEncoder(void)
{
	PMD1bits.TMR1MD = 0;
	PMD3bits.CCP1MD = 0;
	T1CKIPPS = PPS_IN_RC3;
	TMR1H = 0;
	TMR1L = 0;
	T1GCON = 0;
	PIR4bits.TMR1IF = 0;
	T1CLK = T1CLK_USE_T1CKIN_PIN;
	T1CON = _T1CON_ON_MASK | _T1CON_RD16_MASK;

	CCPR1H = 0;
	CCPR1L = 0;
	CCP1CON = _CCP1CON_EN_MASK | CCP1CON_HOLD_IN_RESET_MODE;
}

static inline void configureClcAsOrGateForCcpAndComparator(void)
{
	PMD5bits.CLC2MD = 0;
	CLC2SEL0 = CLCSEL_CURRENT_SENSE_COMPARATOR;
	CLC2SEL1 = CLCSEL_CCP;
	CLC2GLS0 = _CLC2GLS0_LC2G1D1T_MASK | _CLC2GLS0_LC2G1D2T_MASK;
	CLC2GLS1 = 0;
	CLC2GLS2 = 0;
	CLC2GLS3 = 0;
	CLC2POL = 0b10001110;
	CLC2CON = _CLC2CON_LC2EN_MASK | CLCCON_MODE_AND4;
}

static inline void configureCwgForPwmOutputWithClcShutdown(void)
{
	PMD4bits.CWG1MD = 0;
	CWG1AS0 = CWG1AS0_FORCE_ALL_ZERO_ON_SHUTDOWN;
	CWG1AS1 = CWG1AS1_SHUTDOWN_ON_CLC2;
	CWG1STR = 0;
	CWG1CLKCON = CWG1CLKCON_USE_FOSC;
	CWG1DAT = CWG1DAT_USE_PWM4;
	CWG1CON0 = _CWG1CON0_EN_MASK | CWG1CON0_SYNCHRONOUS_STEERING_MODE;
}

static inline void configurePwmAsOffAndComparatorAsCurrentSenseUsingDac(void)
{
	PMD2bits.CMP1MD = 0;
	PMD3bits.PWM4MD = 0;
	CM1NCH = CM1NCH_CURRENT_SENSE_PIN;
	CM1PCH = CM1PCH_DAC;
	CM1CON1 = _CM1CON1_INTP_MASK;
	PIR2bits.C1IF = 0;

	PWM4DCH = 0;
	PWM4DCL = 0;
	PWM4CON = _PWM4CON_PWM4EN_MASK;
	CM1CON0 = _CM1CON0_EN_MASK | _CM1CON0_HYS_MASK | _CM1CON0_POL_MASK;
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
