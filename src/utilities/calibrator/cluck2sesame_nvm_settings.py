from types import SimpleNamespace

_RETLW = 0b11_0100_0000_0000
_TEMPERATURE_ADC_HIGH_OFFSET = 4

class Cluck2SesameNvmSettings:
	def __init__(self, address, nvm):
		if len(nvm) != 32:
			raise ValueError('nvm', 'NVM Settings is 32 words but received ' + len(nvm) + ' words')

		self._address = address
		self._raw = nvm.copy()
		self._lcd_contrast = self._uint8_at(0)
		self._temperature_adc_high = self._uint16_at(_TEMPERATURE_ADC_HIGH_OFFSET)
		self._flags = self._uint8_at(12)

	@property
	def address(self): return self._address

	@property
	def raw(self): return self._raw.copy()

	@property
	def lcd_contrast(self): return self._lcd_contrast

	@property
	def temperature_adc_high(self): return self._temperature_adc_high

	@property
	def flags(self): return Cluck2SesameNvmSettings._decode_flags(self._flags)

	def _uint8_at(self, offset):
		word = self._raw[offset]
		if (word & _RETLW) != _RETLW:
			raise Exception('NVM Settings at offset ' + offset + ' is not a retlw instruction')

		return word & 0xff

	def _uint16_at(self, offset):
		return self._uint8_at(offset) | (self._uint8_at(offset + 1) << 8)

	@staticmethod
	def _decode_flags(flags):
		as_obj = SimpleNamespace(raw=flags)
		if flags & 0b0000_0001:
			as_obj.is_calibration_required = True

		return as_obj

	def with_temperature_adc_high(self, value):
		nvm = self._raw.copy()
		nvm[_TEMPERATURE_ADC_HIGH_OFFSET + 0] = _RETLW | ((value >> 0) & 0xff)
		nvm[_TEMPERATURE_ADC_HIGH_OFFSET + 1] = _RETLW | ((value >> 8) & 0xff)
		return Cluck2SesameNvmSettings(self._address, nvm)
