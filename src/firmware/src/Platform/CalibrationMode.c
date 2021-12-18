#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define __PERIODICMONITOR_EXPOSE_INTERNALS

#include "Event.h"
#include "NvmSettings.h"
#include "PowerManagement.h"
#include "PeriodicMonitor.h"
#include "CalibrationMode.h"

#define PPS_IN_RB7 0x0f
#define PPS_OUT_CLKR 0x1b
#define PPS_OUT_UART1TX 0x0f

#define CLKRCON_DUTY_CYCLE_50 (0b10 << _CLKRCON_CLKRDC_POSITION)
#define CLKRCON_NO_DIVIDER 0
#define CLKRCLK_SOURCE_SOSC (0b0101 << _CLKRCLK_CLKRCLK_POSITION)

static void configureReferenceClockModuleFor32768HzCrystalOutput(void);
static void configureUart1AsAsynchronous8bit9600BaudContinuousReception(void);
static void onWokenFromSleep(const struct Event *event);
static void tryToTransmitNextByteToHost(void);
static void onNoCommandReceived(void);
static void transmitToHost(const uint8_t buffer[]);
static void onSampleParametersCommandReceived(void);
static void hexDigitsForByte(uint8_t *out, uint8_t byte);
static uint8_t hexDigitHigh(uint8_t value);
static uint8_t hexDigitLow(uint8_t value);
static void hexDigitsForWord(uint8_t *out, uint16_t word);
static void onUnknownCommandReceived(void);

static uint8_t transmitBuffer[] = {
	CALIBRATIONMODE_CMD_EOL, CALIBRATIONMODE_CMD_EOL, CALIBRATIONMODE_CMD_EOL, CALIBRATIONMODE_CMD_EOL,
	CALIBRATIONMODE_CMD_EOL, CALIBRATIONMODE_CMD_EOL, CALIBRATIONMODE_CMD_EOL, CALIBRATIONMODE_CMD_EOL,
	CALIBRATIONMODE_CMD_EOL, CALIBRATIONMODE_CMD_EOL, CALIBRATIONMODE_CMD_EOL, CALIBRATIONMODE_CMD_EOL,
	CALIBRATIONMODE_CMD_EOL, CALIBRATIONMODE_CMD_EOL, CALIBRATIONMODE_CMD_EOL, CALIBRATIONMODE_CMD_EOL
};

static const uint8_t *transmitBufferPtr;

void calibrationModeInitialise(void)
{
	LATBbits.LATB6 = 0;
	LATBbits.LATB7 = 0;
	ANSELBbits.ANSB6 = 0;
	ANSELBbits.ANSB7 = 0;
	if (!nvmSettings.platform.flags.bits.isCalibrationRequired)
	{
		PMD4bits.UART1MD = 1;
		PMD0bits.CLKRMD = 1;
		RB6PPS = 0;
		RB7PPS = 0;
		RX1DTPPS = 0;
		ODCONBbits.ODCB7 = 0;
		TRISBbits.TRISB6 = 0;
		TRISBbits.TRISB7 = 0;
		return;
	}

	configureReferenceClockModuleFor32768HzCrystalOutput();
	configureUart1AsAsynchronous8bit9600BaudContinuousReception();

	static const struct EventSubscription onWokenFromSleepSubscription =
	{
		.type = WOKEN_FROM_SLEEP,
		.handler = &onWokenFromSleep,
		.state = (void *) 0
	};

	eventSubscribe(&onWokenFromSleepSubscription);
}

static void configureReferenceClockModuleFor32768HzCrystalOutput(void)
{
	PMD0bits.CLKRMD = 0;
	TRISBbits.TRISB6 = 0;
	CLKRCON = CLKRCON_DUTY_CYCLE_50 | CLKRCON_NO_DIVIDER;
	CLKRCLK = CLKRCLK_SOURCE_SOSC;
	RB6PPS = PPS_OUT_CLKR;
}

static void configureUart1AsAsynchronous8bit9600BaudContinuousReception(void)
{
	PMD4bits.UART1MD = 0;
	TRISBbits.TRISB7 = 0;
	ODCONBbits.ODCB7 = 1;
	RB7PPS = PPS_OUT_UART1TX;
	RX1DTPPS = PPS_IN_RB7;
	SP1BRG = 51;
	TX1STA = _TX1STA_TXEN_MASK;
	RC1STA = _RC1STA_CREN_MASK | _RC1STA_SPEN_MASK;
	BAUD1CON = 0;
	PIE3bits.RC1IE = 1;
	PIE3bits.TX1IE = 0;
}

static void onWokenFromSleep(const struct Event *event)
{
	static uint8_t buffer[4];
	static uint8_t bufferIndex = 0;
	static bool isTransmitting = false;
	while (PIR3bits.RC1IF)
	{
		uint8_t received = RC1REG;
		if (isTransmitting)
		{
			isTransmitting = received != CALIBRATIONMODE_CMD_EOL;
			if (isTransmitting)
				tryToTransmitNextByteToHost();
		}
		else if (received == CALIBRATIONMODE_CMD_EOL)
		{
			if (bufferIndex == 0)
				onNoCommandReceived();
			else if (buffer[0] == CALIBRATIONMODE_CMD_SAMPLEPARAMETERS && bufferIndex == 1)
				onSampleParametersCommandReceived();
			else
				onUnknownCommandReceived();

			bufferIndex = 0;
			isTransmitting = true;
		}
		else if (bufferIndex < sizeof(buffer))
		{
			buffer[bufferIndex] = received;
			bufferIndex++;
		}
		else
			bufferIndex = 0xff;
	}
}

static void tryToTransmitNextByteToHost(void)
{
	if (TX1STAbits.TRMT)
	{
		TX1REG = *transmitBufferPtr;
		transmitBufferPtr++;
	}
}

static void onNoCommandReceived(void)
{
	static const uint8_t ok[] = {CALIBRATIONMODE_REPLY_OK, 'K', CALIBRATIONMODE_CMD_EOL};
	transmitToHost(ok);
}

static void transmitToHost(const uint8_t buffer[])
{
	transmitBufferPtr = buffer;
	tryToTransmitNextByteToHost();
}

static void onSampleParametersCommandReceived(void)
{
	struct MonitoredParametersSampled sample;
	periodicMonitorSampleNow(&sample);
	hexDigitsForByte(transmitBuffer, sample.timestamp);
	transmitBuffer[2] = ',';
	hexDigitsForByte(transmitBuffer + 3, sample.flags.all);
	transmitBuffer[5] = ',';
	hexDigitsForWord(transmitBuffer + 6, sample.fvr);
	transmitBuffer[10] = ',';
	hexDigitsForWord(transmitBuffer + 11, sample.temperature);
	transmitBuffer[15] = CALIBRATIONMODE_CMD_EOL;
	transmitToHost(transmitBuffer);
}

static void hexDigitsForByte(uint8_t *out, uint8_t byte) // TODO: MORE WIDELY USEFUL (FOR EXAMPLE, USED IN THE TESTS) - EXTRACT SOMEWHERE
{
	*(out++) = hexDigitHigh(byte);
	*out = hexDigitLow(byte);
}

static uint8_t hexDigitHigh(uint8_t value)
{
	return hexDigitLow((value >> 4) & 0x0f);
}

static uint8_t hexDigitLow(uint8_t value)
{
	value &= 0x0f;
	if (value > 9)
		return 'a' + (value - 10);

	return '0' + value;
}

static void hexDigitsForWord(uint8_t *out, uint16_t word)
{
	hexDigitsForByte(out, (uint8_t) (word >> 8));
	hexDigitsForByte(out + 2, (uint8_t) word);
}

static void onUnknownCommandReceived(void)
{
	static const uint8_t error[] = {CALIBRATIONMODE_REPLY_ERROR, '0', '1', CALIBRATIONMODE_CMD_EOL};
	transmitToHost(error);
}
