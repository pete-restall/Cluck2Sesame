#ifndef __CLUCK3SESAME_TESTS_PLATFORM_CALIBRATIONMODE_CALIBRATIONMODEFIXTURE_H
#define __CLUCK3SESAME_TESTS_PLATFORM_CALIBRATIONMODE_CALIBRATIONMODEFIXTURE_H
#include <stdint.h>

extern void calibrationModeFixtureSetUp(void);
extern void calibrationModeFixtureTearDown(void);
extern void stubNvmSettingsWithCalibrationRequired(void);
extern void stubNvmSettingsWithoutCalibrationRequired(void);
extern void assertWokenFromSleepSubscription(void);
extern void assertNoWokenFromSleepSubscription(void);

#endif
