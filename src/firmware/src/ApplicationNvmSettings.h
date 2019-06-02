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
			uint16_t height;
		} door;
	};
};

#endif
