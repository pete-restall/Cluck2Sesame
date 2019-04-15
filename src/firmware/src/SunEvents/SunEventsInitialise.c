#include <xc.h>
#include <stdint.h>

#include "SunEvents.h"

extern const __rom void *const sunriseLookupTable;
extern const __rom void *const sunsetLookupTable;

void suneventsInitialise(void)
{
	// TODO: THIS IS BIG CODE, SO ENCAPSULATE AN ENTRY LOOKUP INTO A FUNCTION...
	uint8_t a = ((const uint8_t *) sunriseLookupTable)[0];
	uint8_t b = ((const uint8_t *) sunsetLookupTable)[0];
	uint8_t c = a + b;
}
