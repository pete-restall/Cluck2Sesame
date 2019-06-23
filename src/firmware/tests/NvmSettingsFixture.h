#ifndef __CLUCK3SESAME_TESTS_NVMSETTINGSFIXTURE_H
#define __CLUCK3SESAME_TESTS_NVMSETTINGSFIXTURE_H
#include "Platform/NvmSettings.h"
#include "ApplicationNvmSettings.h"

extern void stubNvmSettings(const union NvmSettings *stubbed);

extern void stubNvmApplicationSettings(
	const union ApplicationNvmSettings *stubbed);

#endif
