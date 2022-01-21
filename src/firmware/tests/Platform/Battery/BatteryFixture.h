#ifndef __CLUCK2SESAME_TESTS_BATTERY_BATTERYFIXTURE_H
#define __CLUCK2SESAME_TESTS_BATTERY_BATTERYFIXTURE_H
#include <stdint.h>

#define FVR_IDEAL_MV 2048
#define FVR_TOLERANCE_MV 20
#define FVR_MINIMUM_VALID_SAMPLE 2768
#define FVR_MAXIMUM_VALID_SAMPLE 6672

extern void batteryFixtureSetUp(void);
extern void batteryFixtureTearDown(void);
extern uint16_t stubAnyDiaFvra2xMillivolts(void);
extern void stubDiaFvra2xMillivolts(uint16_t millivolts);
extern void stubAllParametersThatWillEnableCharging(void);
extern void stubChargerGoodPinLow(void);
extern void stubChargerGoodPinHigh(void);
extern void stubTemperatureWithinChargingRange(void);
extern void stubTemperatureOf(int16_t celsius);
extern void stubBatteryVoltageWithinChargingRange(void);

extern const struct BatteryChargerEnabled *batteryChargerEnabledEventArgs;
extern const struct BatteryChargerDisabled *batteryChargerDisabledEventArgs;

#endif
