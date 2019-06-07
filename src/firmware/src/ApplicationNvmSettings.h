#ifndef __CLUCK3SESAME_SRC_APPLICATIONNVMSETTINGS_H
#define __CLUCK3SESAME_SRC_APPLICATIONNVMSETTINGS_H
#include <stdint.h>

union ApplicationNvmSettings
{
	uint8_t raw[16];
	struct
	{
		struct
		{
			int8_t latitudeOffset;
			int8_t longitudeOffset;
		} location;

		struct
		{
			struct
			{
				unsigned int isManuallyOverridden : 1;
				unsigned int isSunEventDriven : 1;
				unsigned int isTimeDriven : 1;
			} mode;

			struct
			{
				uint8_t hour;
				uint8_t minute;
			} autoOpenTime;

			struct
			{
				uint8_t hour;
				uint8_t minute;
			} autoCloseTime;

			uint16_t height;
		} door;
	};
};

#endif
