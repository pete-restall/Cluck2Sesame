#ifndef __CLUCK3SESAME_TESTS_PLATFORM_CALIBRATIONMODE_CALIBRATIONMODEFIXTUREWITHUART_H
#define __CLUCK3SESAME_TESTS_PLATFORM_CALIBRATIONMODE_CALIBRATIONMODEFIXTUREWITHUART_H
#include <stdint.h>
#include <stddef.h>

extern volatile uint8_t deviceToHostBytes[16];
extern volatile uint8_t deviceToHostNumberOfBytes;

extern void calibrationModeFixtureSetUp(void);
extern void calibrationModeFixtureTearDown(void);
extern void stubNvmSettingsWithCalibrationRequired(void);
extern void stubNvmSettingsWithoutCalibrationRequired(void);
extern void fakeHostToDeviceSend(const uint8_t *bytes, size_t numberOfBytes);
extern void fakeHostWaitForDeviceResponse(void);

#endif
