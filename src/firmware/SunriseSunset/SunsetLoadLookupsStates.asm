	#include "Platform.inc"
	#include "FarCalls.inc"
	#include "Flash.inc"
	#include "SunriseSunset.inc"
	#include "States.inc"

	radix decimal

	defineSunriseSunsetState SUN_STATE_SUNSET_LOADLOOKUPS
		call loadLookupIndexIntoFlashAddress
		call loadLookupTableEntryFromFlash

		setSunriseSunsetNextState SUN_STATE_SUNSET_LOADLOOKUPS2
		setSunriseSunsetState SUN_STATE_LOADLOOKUPS
		returnFromSunriseSunsetState


	defineSunriseSunsetStateInSameSection SUN_STATE_SUNSET_LOADLOOKUPS2
		call loadLookupIndexIntoFlashAddress
		call loadLookupTableEntryFromFlash

		setSunriseSunsetStoreState SUN_STATE_SUNSET_STORE
		setSunriseSunsetState SUN_STATE_LOADLOOKUPS3
		returnFromSunriseSunsetState


loadLookupIndexIntoFlashAddress:
	.knownBank sunriseSunsetState
	.setBankFor lookupIndexLow
	bcf STATUS, C
	rlf lookupIndexLow, W
	.setBankFor EEADR
	addlw low(sunsetLookupTable)
	movwf EEADR

	.setBankFor EEADRH
	movlw high(sunsetLookupTable)
	movwf EEADRH
	btfsc STATUS, C
	incf EEADRH

	.setBankFor lookupIndexHigh
	rlf lookupIndexLow, W
	rlf lookupIndexHigh, W
	.setBankFor EEADRH
	addwf EEADRH

	return

	end
