#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <unity.h>

#include "Platform/HexDigits.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/HexDigits.c")

static uint8_t hexDigitForHighNybbleOf(uint8_t byte);
static uint8_t hexDigitForLowNybbleOf(uint8_t byte);
static uint16_t anyCasedFourHexDigitsInto(uint8_t *outFourDigits);
static uint8_t anyNonHexDigit(void);

static const uint8_t *hexDigits = (const uint8_t *) "0123456789abcdef";

void onBeforeTest(void)
{
}

void onAfterTest(void)
{
}

void test_hexDigitsForByte_called_expectExpectTwoCorrectHexDigits(void)
{
	for (uint16_t i = 0; i < 256; i++)
	{
		uint8_t digits[2];
		uint8_t byte = (uint8_t) i;
		hexDigitsForByte(digits, byte);
		TEST_ASSERT_EQUAL_MESSAGE(hexDigitForHighNybbleOf(byte), digits[0], "0");
		TEST_ASSERT_EQUAL_MESSAGE(hexDigitForLowNybbleOf(byte), digits[1], "1");
	}
}

static uint8_t hexDigitForHighNybbleOf(uint8_t byte)
{
	return hexDigits[(byte >> 4) & 0x0f];
}

static uint8_t hexDigitForLowNybbleOf(uint8_t byte)
{
	return hexDigits[byte & 0x0f];
}

void test_hexDigitsForWord_called_expectExpectFourCorrectHexDigits(void)
{
	for (uint32_t i = 0; i < 65536l; i++)
	{
		uint8_t digits[4];
		uint16_t word = (uint16_t) i;
		hexDigitsForWord(digits, word);
		TEST_ASSERT_EQUAL_MESSAGE(hexDigitForHighNybbleOf((uint8_t) (word >> 8)), digits[0], "0");
		TEST_ASSERT_EQUAL_MESSAGE(hexDigitForLowNybbleOf((uint8_t) (word >> 8)), digits[1], "1");
		TEST_ASSERT_EQUAL_MESSAGE(hexDigitForHighNybbleOf((uint8_t) word), digits[2], "2");
		TEST_ASSERT_EQUAL_MESSAGE(hexDigitForLowNybbleOf((uint8_t) word), digits[3], "3");
	}
}

void test_hexDigitsToWord_calledWithNullOutWord_expectFalseIsReturned(void)
{
	uint8_t inFourDigits[4];
	anyCasedFourHexDigitsInto(inFourDigits);
	bool wasSuccessful = hexDigitsToWord((uint16_t *) 0, inFourDigits);
	TEST_ASSERT_FALSE(wasSuccessful);
}

static uint16_t anyCasedFourHexDigitsInto(uint8_t *outFourDigits)
{
	if (!outFourDigits)
		return 0;

	uint16_t equivalentWord = 0;
	for (uint8_t i = 0; i < 4; i++, outFourDigits++)
	{
		uint16_t anyNybble = anyByteLessThan(16);
		equivalentWord <<= 4;
		equivalentWord |= anyNybble;

		*outFourDigits = hexDigits[anyNybble];
		if (*outFourDigits >= 'a' && anyBoolean())
			*outFourDigits = *outFourDigits - 'a' + 'A';
	}

	return equivalentWord;
}

void test_hexDigitsToWord_calledWithNullInFourDigits_expectFalseIsReturned(void)
{
	uint16_t outWord;
	bool wasSuccessful = hexDigitsToWord(&outWord, (uint8_t *) 0);
	TEST_ASSERT_FALSE(wasSuccessful);
}

void test_hexDigitsToWord_calledWithNullInFourDigits_expectOutWordIsNotModified(void)
{
	uint16_t outWord = anyWord();
	uint16_t originalOutWord = outWord;
	hexDigitsToWord(&outWord, (uint8_t *) 0);
	TEST_ASSERT_EQUAL_UINT16(originalOutWord, outWord);
}

void test_hexDigitsToWord_calledWithInvalidFirstHexDigit_expectFalseIsReturned(void)
{
	uint8_t invalidInFourDigits[4];
	anyCasedFourHexDigitsInto(invalidInFourDigits);
	invalidInFourDigits[0] = anyNonHexDigit();

	uint16_t outWord;
	bool wasSuccessful = hexDigitsToWord(&outWord, invalidInFourDigits);
	TEST_ASSERT_FALSE(wasSuccessful);
}

static uint8_t anyNonHexDigit(void)
{
	while (true)
	{
		uint8_t nonHexDigit = anyByte();
		if (nonHexDigit < '0' || nonHexDigit > 'f')
			return nonHexDigit;

		if (nonHexDigit > '9' && nonHexDigit < 'A')
			return nonHexDigit;

		if (nonHexDigit > 'F' && nonHexDigit < 'a')
			return nonHexDigit;
	}
}

void test_hexDigitsToWord_calledWithInvalidFirstHexDigit_expectOutWordIsNotModified(void)
{
	uint8_t invalidInFourDigits[4];
	anyCasedFourHexDigitsInto(invalidInFourDigits);
	invalidInFourDigits[0] = anyNonHexDigit();

	uint16_t outWord = anyWord();
	uint16_t originalOutWord = outWord;
	hexDigitsToWord(&outWord, invalidInFourDigits);
	TEST_ASSERT_EQUAL_UINT16(originalOutWord, outWord);
}

void test_hexDigitsToWord_calledWithInvalidSecondHexDigit_expectFalseIsReturned(void)
{
	uint8_t invalidInFourDigits[4];
	anyCasedFourHexDigitsInto(invalidInFourDigits);
	invalidInFourDigits[1] = anyNonHexDigit();

	uint16_t outWord;
	bool wasSuccessful = hexDigitsToWord(&outWord, invalidInFourDigits);
	TEST_ASSERT_FALSE(wasSuccessful);
}

void test_hexDigitsToWord_calledWithInvalidSecondHexDigit_expectOutWordIsNotModified(void)
{
	uint8_t invalidInFourDigits[4];
	anyCasedFourHexDigitsInto(invalidInFourDigits);
	invalidInFourDigits[1] = anyNonHexDigit();

	uint16_t outWord = anyWord();
	uint16_t originalOutWord = outWord;
	hexDigitsToWord(&outWord, invalidInFourDigits);
	TEST_ASSERT_EQUAL_UINT16(originalOutWord, outWord);
}

void test_hexDigitsToWord_calledWithInvalidThirdHexDigit_expectFalseIsReturned(void)
{
	uint8_t invalidInFourDigits[4];
	anyCasedFourHexDigitsInto(invalidInFourDigits);
	invalidInFourDigits[2] = anyNonHexDigit();

	uint16_t outWord;
	bool wasSuccessful = hexDigitsToWord(&outWord, invalidInFourDigits);
	TEST_ASSERT_FALSE(wasSuccessful);
}

void test_hexDigitsToWord_calledWithInvalidThirdHexDigit_expectOutWordIsNotModified(void)
{
	uint8_t invalidInFourDigits[4];
	anyCasedFourHexDigitsInto(invalidInFourDigits);
	invalidInFourDigits[2] = anyNonHexDigit();

	uint16_t outWord = anyWord();
	uint16_t originalOutWord = outWord;
	hexDigitsToWord(&outWord, invalidInFourDigits);
	TEST_ASSERT_EQUAL_UINT16(originalOutWord, outWord);
}

void test_hexDigitsToWord_calledWithInvalidFourthHexDigit_expectFalseIsReturned(void)
{
	uint8_t invalidInFourDigits[4];
	anyCasedFourHexDigitsInto(invalidInFourDigits);
	invalidInFourDigits[3] = anyNonHexDigit();

	uint16_t outWord;
	bool wasSuccessful = hexDigitsToWord(&outWord, invalidInFourDigits);
	TEST_ASSERT_FALSE(wasSuccessful);
}

void test_hexDigitsToWord_calledWithInvalidFourthHexDigit_expectOutWordIsNotModified(void)
{
	uint8_t invalidInFourDigits[4];
	anyCasedFourHexDigitsInto(invalidInFourDigits);
	invalidInFourDigits[3] = anyNonHexDigit();

	uint16_t outWord = anyWord();
	uint16_t originalOutWord = outWord;
	hexDigitsToWord(&outWord, invalidInFourDigits);
	TEST_ASSERT_EQUAL_UINT16(originalOutWord, outWord);
}

void test_hexDigitsToWord_calledWithValidHexDigits_expectTrueIsReturned(void)
{
	uint16_t outWord;
	uint8_t inFourDigits[5] = {0};
	anyCasedFourHexDigitsInto(inFourDigits);
	bool wasSuccessful = hexDigitsToWord(&outWord, inFourDigits);
	TEST_ASSERT_TRUE_MESSAGE(wasSuccessful, (char *) inFourDigits);
}

void test_hexDigitsToWord_calledWithValidHexDigits_expectCorrectWordIsReturned(void)
{
	for (uint8_t i = 0; i < 10; i++)
	{
		uint16_t outWord;
		uint8_t inFourDigits[5] = {0};
		uint16_t correctWord = anyCasedFourHexDigitsInto(inFourDigits);
		hexDigitsToWord(&outWord, inFourDigits);
		TEST_ASSERT_EQUAL_UINT16_MESSAGE(correctWord, outWord, (char *) inFourDigits);
	}
}
