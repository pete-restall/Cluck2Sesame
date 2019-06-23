#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Platform/NvmSettings.h"
#include "Platform/Motor.h"

#include "MotorFixture.h"

#include "../../Fixture.h"
#include "../../NvmSettingsFixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/Motor/MotorInitialise.c")
TEST_FILE("Platform/Motor/MotorEnableDisable.c")
TEST_FILE("Platform/Motor/MotorOnOff.c")
TEST_FILE("Platform/Motor/MotorSetLimits.c")

void onBeforeTest(void)
{
	motorFixtureSetUp();
	motorInitialise();

	union NvmSettings replacementSettings =
	{
		.platform =
		{
			.motor =
			{
				.currentLimitNoLoad = anyByteLessThan(32),
				.currentLimitMaximumLoad = anyByteLessThan(32)
			}
		}
	};

	stubNvmSettings(&replacementSettings);
}

void onAfterTest(void)
{
	motorFixtureTearDown();
}

void test_motorLimitIsNoLoad_called_expectCurrentLimitIsFromNvmSettings(void)
{
	ensureMotorFullyEnabled();
	DAC1CON1 = anyByteExcept(nvmSettings.platform.motor.currentLimitNoLoad);

	motorLimitIsNoLoad();
	TEST_ASSERT_EQUAL_UINT8(
		nvmSettings.platform.motor.currentLimitNoLoad,
		DAC1CON1);
}

void test_motorLimitIsMaximumLoad_called_expectCurrentLimitIsFromNvmSettings(void)
{
	ensureMotorFullyEnabled();
	DAC1CON1 = anyByteExcept(nvmSettings.platform.motor.currentLimitMaximumLoad);

	motorLimitIsMaximumLoad();
	TEST_ASSERT_EQUAL_UINT8(
		nvmSettings.platform.motor.currentLimitMaximumLoad,
		DAC1CON1);
}
