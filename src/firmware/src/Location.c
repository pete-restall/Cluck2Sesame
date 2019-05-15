#include <xc.h>

#include "Location.h"

static void buggyCompilerWorkaround(void)
{
	static const struct LocationChanged dummy =
	{
		.location = _OMNITARGET
	};
}
