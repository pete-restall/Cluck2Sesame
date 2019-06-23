#include <xc.h>
#include <stdint.h>

#include "../NvmSettings.h"

#include "Motor.h"

void motorLimitIsNoLoad(void)
{
	DAC1CON1 = nvmSettings.platform.motor.currentLimitNoLoad;
}

void motorLimitIsMaximumLoad(void)
{
	DAC1CON1 = nvmSettings.platform.motor.currentLimitMaximumLoad;
}
