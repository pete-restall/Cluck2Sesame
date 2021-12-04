#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Adc.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Adc.c")

#define RESULT_ADDRL(x) __at(0x23a0 + (x) * 2 + 0)
#define RESULT_ADDRH(x) __at(0x23a0 + (x) * 2 + 1)

static uint8_t anyChannel(void);
static uint16_t anyRightJustifiedAdcResult(void);

volatile uint8_t isInvalidSession;
volatile uint8_t adcon0BeforeConversion;
volatile uint8_t adcon1BeforeConversion;
volatile uint8_t adactBeforeConversion;
volatile uint8_t stubAdcResultIndex;
volatile uint16_t stubAdcResult[8] RESULT_ADDRL(0);
volatile uint8_t stubAdcResult0l RESULT_ADDRL(0);
volatile uint8_t stubAdcResult0h RESULT_ADDRH(0);
volatile uint8_t stubAdcResult1l RESULT_ADDRL(1);
volatile uint8_t stubAdcResult1h RESULT_ADDRH(1);
volatile uint8_t stubAdcResult2l RESULT_ADDRL(2);
volatile uint8_t stubAdcResult2h RESULT_ADDRH(2);
volatile uint8_t stubAdcResult3l RESULT_ADDRL(3);
volatile uint8_t stubAdcResult3h RESULT_ADDRH(3);
volatile uint8_t stubAdcResult4l RESULT_ADDRL(4);
volatile uint8_t stubAdcResult4h RESULT_ADDRH(4);
volatile uint8_t stubAdcResult5l RESULT_ADDRL(5);
volatile uint8_t stubAdcResult5h RESULT_ADDRH(5);
volatile uint8_t stubAdcResult6l RESULT_ADDRL(6);
volatile uint8_t stubAdcResult6h RESULT_ADDRH(6);
volatile uint8_t stubAdcResult7l RESULT_ADDRL(7);
volatile uint8_t stubAdcResult7h RESULT_ADDRH(7);

static const uint8_t maxStubResults = sizeof(stubAdcResult) / sizeof(uint16_t);

void onBeforeTest(void)
{
	isInvalidSession = 0;
	adcon0BeforeConversion = 0;
	adcon1BeforeConversion = 0;
	adactBeforeConversion = 0;
	stubAdcResultIndex = 0;

	PMD2bits.ADCMD = 1;
	adcInitialise();
}

void onAfterTest(void)
{
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, isInvalidSession, "ADC violations !");
}

void test_adcSample_calledWithNullBuffer_expectReturnsWithoutTrap(void)
{
	adcSample((struct AdcSample *) 0);
}

void test_adcSample_calledWithNullBuffer_expectAdcModuleIsNotEnabled(void)
{
	PMD2 = anyByteWithMaskSet(_PMD2_ADCMD_MASK);
	uint8_t originalPmd2 = PMD2;
	adcSample((struct AdcSample *) 0);
	TEST_ASSERT_EQUAL_UINT8(originalPmd2, PMD2);
}

void test_adcSample_called_expectAdcModuleIsDisabledAfterSampling(void)
{
	PMD2 = anyByteWithMaskClear(_PMD2_ADCMD_MASK);
	uint8_t originalPmd2 = PMD2;
	struct AdcSample sample = { .count = 1, .channel = anyChannel(), .flags = { .all = 0 } };
	adcSample(&sample);
	TEST_ASSERT_EQUAL_UINT8(originalPmd2 | _PMD2_ADCMD_MASK, PMD2);
}

static uint8_t anyChannel(void)
{
	uint8_t channel = anyByteWithMaskSet(_ADCON0_ADON_MASK);
	uint8_t chs = channel & _ADCON0_CHS_MASK;
	if (chs >= 0b01100000 || chs <= 0b11101000)
		channel |= 0b11110000;

	return channel & ~_ADCON0_GO_MASK;
}

void test_adcSample_called_expectResultIsFromAdres(void)
{
	stubAdcResult[0] = anyRightJustifiedAdcResult();
	struct AdcSample sample =
	{
		.count = 1,
		.channel = anyChannel(),
		.result = anyWordExcept(stubAdcResult[0]),
		.flags = { .all = 0 }
	};

	adcSample(&sample);
	TEST_ASSERT_EQUAL_UINT16(stubAdcResult[0], sample.result);
}

static uint16_t anyRightJustifiedAdcResult(void)
{
	return anyWord() & 0x03ff;
}

void test_adcSample_called_expectAdcChannelIsSameAsPassedIn(void)
{
	ADCON0 = anyByte();
	uint8_t channel = anyChannel();
	struct AdcSample sample = { .count = 1, .channel = channel, .flags = { .all = 0 } };
	adcSample(&sample);
	TEST_ASSERT_EQUAL_UINT8(
		channel & _ADCON0_CHS_MASK,
		adcon0BeforeConversion & _ADCON0_CHS_MASK);
}

void test_adcSample_called_expectAdcIsTurnedOn(void)
{
	ADCON0 = anyByteWithMaskClear(_ADCON0_GO_MASK);
	struct AdcSample sample = { .count = 1, .channel = anyChannel(), .flags = { .all = 0 } };
	adcSample(&sample);
	TEST_ASSERT_BITS_HIGH(_ADCON0_ADON_MASK, adcon0BeforeConversion);
}

void test_adcSample_called_expectAdcResultIsRightJustified(void)
{
	ADCON1 = anyByte();
	struct AdcSample sample = { .count = 1, .channel = anyChannel() };
	adcSample(&sample);
	TEST_ASSERT_BITS_HIGH(_ADCON1_ADFM_MASK, adcon1BeforeConversion);
}

void test_adcSample_called_expectAdcConversionClockIsFoscOver64For2MicrosecondClockPeriod(void)
{
	ADCON1 = anyByte();
	struct AdcSample sample = { .count = 1, .channel = anyChannel(), .flags = { .all = 0 } };
	adcSample(&sample);
	TEST_ASSERT_EQUAL_UINT8(
		0b110,
		(adcon1BeforeConversion >> _ADCON1_ADCS_POSITION) & 0b111);
}

void test_adcSample_calledWhenVrefIsFvrFlagIsClear_expectAdcReferenceVoltageIsVdd(void)
{
	ADCON1 = anyByte();
	struct AdcSample sample = { .count = 1, .channel = anyChannel(), .flags = { .all = 0 } };
	adcSample(&sample);
	TEST_ASSERT_EQUAL_UINT8(
		0b00,
		(adcon1BeforeConversion >> _ADCON1_ADPREF_POSITION) & 0b11);
}

void test_adcSample_calledWhenVrefIsFvrFlagIsSet_expectAdcReferenceVoltageIsInternalFixedVoltageReference(void)
{
	ADCON1 = anyByte();
	struct AdcSample sample = { .count = 1, .channel = anyChannel(), .flags = { .all = 0 } };
	sample.flags.vrefIsFvr = 1;
	adcSample(&sample);
	TEST_ASSERT_EQUAL_UINT8(
		0b11,
		(adcon1BeforeConversion >> _ADCON1_ADPREF_POSITION) & 0b11);
}

void test_adcSample_called_expectAdcAutoConversionTriggersAreDisabled(void)
{
	ADACT = anyByte();
	struct AdcSample sample = { .count = 1, .channel = anyChannel(), .flags = { .all = 0 } };
	adcSample(&sample);
	TEST_ASSERT_EQUAL_UINT8(0, adactBeforeConversion);
}

void test_adcSample_calledWithZeroCount_expect0Result(void)
{
	stubAdcResult[0] = anyWordExcept(0);
	struct AdcSample sample =
	{
		.count = 0,
		.channel = anyChannel(),
		.result = anyWordExcept(0),
		.flags = { .all = 0 }
	};

	adcSample(&sample);
	TEST_ASSERT_EQUAL_UINT16(0, sample.result);
}

void test_adcSample_calledWithCountGreaterThanOne_expectAccumulationOfResults(void)
{
	uint8_t count = anyByteLessThan(maxStubResults + 1);
	uint16_t accumulated = 0;
	for (uint8_t i = 0; i < count; i++)
	{
		uint16_t result = anyRightJustifiedAdcResult();
		stubAdcResult[i] = result;
		accumulated += result;
	}

	struct AdcSample sample =
	{
		.count = count,
		.channel = anyChannel(),
		.result = anyWord(),
		.flags = { .all = 0 }
	};

	adcSample(&sample);
	TEST_ASSERT_EQUAL_UINT16(accumulated, sample.result);
}

void test_adcSample_called_expectCountIsNotModified(void)
{
	uint8_t count = anyByteLessThan(maxStubResults + 1);
	struct AdcSample sample = { .count = count, .channel = anyChannel(), .flags = { .all = 0 } };
	adcSample(&sample);
	TEST_ASSERT_EQUAL_UINT8(count, sample.count);
}

void test_adcSample_called_expectChannelIsNotModified(void)
{
	uint8_t channel = anyChannel();
	struct AdcSample sample = { .count = 1, .channel = channel, .flags = { .all = 0 } };
	adcSample(&sample);
	TEST_ASSERT_EQUAL_UINT8(channel, sample.channel);
}

void test_adcSample_called_expectFlagsAreNotModified(void)
{
	uint8_t flags = anyByte();
	struct AdcSample sample = { .count = 1, .channel = anyChannel(), .flags = { .all = flags } };
	adcSample(&sample);
	TEST_ASSERT_EQUAL_UINT8(flags, sample.flags.all);
}
