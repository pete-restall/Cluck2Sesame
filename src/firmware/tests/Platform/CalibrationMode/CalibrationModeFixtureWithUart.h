#ifndef __CLUCK2SESAME_TESTS_PLATFORM_CALIBRATIONMODE_CALIBRATIONMODEFIXTUREWITHUART_H
#define __CLUCK2SESAME_TESTS_PLATFORM_CALIBRATIONMODE_CALIBRATIONMODEFIXTUREWITHUART_H
#include <stdint.h>
#include <stddef.h>

extern volatile uint8_t deviceToHostBytes[18];
extern volatile uint8_t deviceToHostNumberOfBytes;

extern void calibrationModeFixtureSetUp(void);
extern void calibrationModeFixtureTearDown(void);
extern void stubNvmSettingsWithCalibrationRequired(void);
extern void stubNvmSettingsWithoutCalibrationRequired(void);
extern void fakeHostToDeviceSend(const uint8_t *bytes, size_t numberOfBytes);
extern void fakeHostWaitForDeviceResponse(void);
extern void uart1_receivesInvalidCommand_expectInvalidCommandErrorIsTransmittedToHost(const uint8_t *command, size_t numberOfBytes);
extern void uart1_receivesInvalidCommand_expectInvalidArgumentErrorIsTransmittedToHost(const uint8_t *command, size_t numberOfBytes);

#endif
