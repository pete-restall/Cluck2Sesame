#include <xc.h>
#include <stdint.h>

#include "Platform/Event.h"
#include "Motor.h"
#include "Door.h"

#include "DoorFixture.h"
#include "DoorFindBottomFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

const struct MotorStopped raisingStoppedImmediately =
{
	.actualCount = 0,
	.requestedCount = 1234,
	.fault = { .currentLimited = 1 }
};

const struct MotorStopped raisingStoppedAtThreshold =
{
	.requestedCount = 1234,
	.actualCount = PULSES_PER_2MM,
	.fault = { .currentLimited = 1 }
};

const struct MotorStopped raisingStoppedJustAfterThreshold =
{
	.requestedCount = 1234,
	.actualCount = PULSES_PER_2MM + 1,
	.fault = { .currentLimited = 1 }
};

struct MotorStopped raisingStoppedAfterThreshold =
{
	.requestedCount = 1234,
	.actualCount = -1,
	.fault = { .currentLimited = 1 }
};

void onBeforeTest(void)
{
	doorFixtureInitialise();
	raisingStoppedAfterThreshold.actualCount =
		PULSES_PER_2MM + 1 + (int16_t) anyWordLessThan(10000);
}

void onAfterTest(void)
{
	doorFixtureShutdown();
}
