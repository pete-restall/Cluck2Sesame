#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/Clock.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Clock/ClockInitialise.c")
TEST_FILE("Platform/Clock/ClockGetSetNow.c")

void onBeforeTest(void)
{
	eventInitialise();
}

void onAfterTest(void)
{
}

void test_clockInitialise_called_expectTimer0ModuleIsEnabled(void)
{
	PMD1 = anyByteWithMaskSet(_PMD1_TMR0MD_MASK);
	uint8_t originalPmd1 = PMD1;
	clockInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPmd1 & ~_PMD1_TMR0MD_MASK, PMD1);
}

void test_clockInitialise_called_expectOscillatorIsEnabled(void)
{
	OSCEN = anyByteWithMaskClear(_OSCEN_SOSCEN_MASK);
	uint8_t originalOscen = OSCEN;

	clockInitialise();
	TEST_ASSERT_EQUAL_UINT8(OSCEN | _OSCEN_SOSCEN_MASK, OSCEN);
}

void test_clockInitialise_called_expectOscillatorIsReadyToBeUsed(void)
{
	clockInitialise();
	TEST_ASSERT_TRUE(OSCSTATbits.SOR);
}

void test_clockInitialise_called_expectClockSourceIsOscillator(void)
{
	T0CON1 = anyByte();
	clockInitialise();

	int source = (T0CON1 & _T0CON1_T0CS_MASK) >> _T0CON1_T0CS_POSITION;
	TEST_ASSERT_EQUAL_INT(0b110, source & 0b111);
}

void test_clockInitialise_called_expectTimerIsAsynchronous(void)
{
	T0CON1 = anyByteWithMaskClear(_T0CON1_T0ASYNC_MASK);
	clockInitialise();
	TEST_ASSERT_TRUE(T0CON1bits.T0ASYNC);
}

void test_clockInitialise_called_expectTimerPrescalerIs32768ToProduce1SecondTicks(void)
{
	T0CON1 = anyByte();
	clockInitialise();

	int prescale = (T0CON1 & _T0CON1_T0CKPS_MASK) >> _T0CON1_T0CKPS_POSITION;
	TEST_ASSERT_EQUAL_INT(0b1111, prescale & 0b1111);
}

void test_clockInitialise_called_expectTimerIsEnabled(void)
{
	T0CON0 = anyByteWithMaskClear(_T0CON0_T0EN_MASK);
	clockInitialise();
	TEST_ASSERT_TRUE(T0CON0bits.T0EN);
}

void test_clockInitialise_called_expectTimerIsConfiguredAs8BitMode(void)
{
	T0CON0 = anyByteWithMaskSet(_T0CON0_T016BIT_MASK);
	clockInitialise();
	TEST_ASSERT_FALSE(T0CON0bits.T016BIT);
}

void test_clockInitialise_called_expectTimerPostscalerIs1Second(void)
{
	T0CON0 = anyByte();
	clockInitialise();

	int postscale = (T0CON0 & _T0CON0_T0OUTPS_MASK) >> _T0CON0_T0OUTPS_POSITION;
	TEST_ASSERT_EQUAL_INT(0, postscale & 0b1111);
}

void test_clockInitialise_called_expectTimerInterruptFlagIsClear(void)
{
	PIR0 = anyByteWithMaskSet(_PIR0_TMR0IF_MASK);
	uint8_t originalPir0 = PIR0;

	clockInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPir0 & ~_PIR0_TMR0IF_MASK, PIR0);
}

void test_clockInitialise_called_expectTimerCanWakeDeviceFromSleep(void)
{
	PIE0 = anyByteWithMaskClear(_PIE0_TMR0IE_MASK);
	uint8_t originalPie0 = PIE0;

	clockInitialise();
	TEST_ASSERT_EQUAL_UINT8(originalPie0 | _PIE0_TMR0IE_MASK, PIE0);
}

void test_clockInitialise_called_expectTimerIsZero(void)
{
	TMR0L = anyByteExcept(0);
	clockInitialise();
	TEST_ASSERT_EQUAL_UINT8(0, TMR0L);
}

void test_clockInitialise_called_expectTimerComparisonIs60Seconds(void)
{
	TMR0H = anyByteExcept(59);
	clockInitialise();
	TEST_ASSERT_EQUAL_UINT8(59, TMR0H);
}

void test_clockInitialise_called_expectNowDateIsInitialisedAsEpoch(void)
{
	clockInitialise();
	struct DateAndTimeGet now =
	{
		.date =
		{
			.year = anyByte(),
			.month = anyByte(),
			.day = anyByte(),
			.dayOfYear = anyWord(),
			.flags = { .all = anyByte() }
		}
	};

	clockGetNowGmt(&now);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.date.year, "Year");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, now.date.month, "Month");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, now.date.day, "Day");
	TEST_ASSERT_EQUAL_UINT16_MESSAGE(0, now.date.dayOfYear, "Day-of-Year");
	TEST_ASSERT_TRUE_MESSAGE(now.date.flags.isLeapYear, "Leap");
	TEST_ASSERT_FALSE_MESSAGE(now.date.flags.isDaylightSavings, "DST");
}

void test_clockInitialise_called_expectNowTimeIsInitialisedAsMidnight(void)
{
	clockInitialise();
	struct DateAndTimeGet now =
	{
		.time =
		{
			.hour = anyByte(),
			.minute = anyByte(),
			.second = anyByte()
		}
	};

	clockGetNowGmt(&now);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.time.hour, "Hour");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.time.minute, "Minute");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, now.time.second, "Second");
}
