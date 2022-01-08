from types import SimpleNamespace

_RETLW = 0b11_0100_0000_0000
_TEMPERATURE_ADC_HIGH_OFFSET = 4
_TEMPERATURE_CELSIUS_HIGH_OFFSET = 6
_TEMPERATURE_COEFFICIENT_OFFSET = 7
_FLAGS_OFFSET = 12
_CRC8_OFFSET = 31

class Cluck2SesameNvmSettings:
	def __init__(self, address, nvm):
		if len(nvm) != 32:
			raise ValueError('nvm', f'NVM Settings is 32 words but received {len(nvm)} words')

		self._address = address
		self._raw = nvm.copy()
		self._lcd_contrast = self._uint8_at(0)
		self._temperature_adc_high = self._uint16_at(_TEMPERATURE_ADC_HIGH_OFFSET)
		self._flags = self._uint8_at(_FLAGS_OFFSET)

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
			raise Exception(f'NVM Settings at offset {offset} is not a retlw instruction')

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
		adc_value = int(value + 0.5)
		if adc_value < 0 or adc_value > 1023:
			raise Exception(f'ADC value must be an unsigned 10-bit integer; value={value}')

		nvm = self._raw.copy()
		nvm[_TEMPERATURE_ADC_HIGH_OFFSET + 0] = _RETLW | ((adc_value >> 0) & 0xff)
		nvm[_TEMPERATURE_ADC_HIGH_OFFSET + 1] = _RETLW | ((adc_value >> 8) & 0xff)
		return Cluck2SesameNvmSettings(self._address, nvm)

	def with_temperature_celsius_high(self, value):
		scaled_value = int((value - 25) * 10 + 0.5)
		if scaled_value < 0 or scaled_value > 255:
			raise Exception(f'Highest Calibration Temperature is higher than the maximum allowed; celsius={value}')

		nvm = self._raw.copy()
		nvm[_TEMPERATURE_CELSIUS_HIGH_OFFSET] = _RETLW | (scaled_value & 0xff)
		return Cluck2SesameNvmSettings(self._address, nvm)

	def with_temperature_coefficient(self, value):
		scaled_value = int(65536 * -value + 0.5)
		if scaled_value <= 0 or scaled_value > 65535:
			raise Exception(f'Temperature coefficient must be negative and in the closed range (-1, 0); coefficient={value}')

		nvm = self._raw.copy()
		nvm[_TEMPERATURE_COEFFICIENT_OFFSET + 0] = _RETLW | ((scaled_value >> 0) & 0xff)
		nvm[_TEMPERATURE_COEFFICIENT_OFFSET + 1] = _RETLW | ((scaled_value >> 8) & 0xff)
		return Cluck2SesameNvmSettings(self._address, nvm)

	def without_calibration_required_flag(self):
		nvm = self._raw.copy()
		nvm[_FLAGS_OFFSET] = _RETLW | (self._flags & 0xfe)
		return Cluck2SesameNvmSettings(self._address, nvm)

	def with_recalculated_crc8(self):
		nvm = self._raw.copy()
		crc8 = 0
		for input in nvm[0:31]:
			crc8 = Cluck2SesameNvmSettings._crc8Next(crc8, (input >> 8) & 0xff)
			crc8 = Cluck2SesameNvmSettings._crc8Next(crc8, (input >> 0) & 0xff)

		crc8 = Cluck2SesameNvmSettings._crc8Next(crc8, (_RETLW >> 8) & 0xff)
		nvm[_CRC8_OFFSET] = _RETLW | crc8
		return Cluck2SesameNvmSettings(self._address, nvm)

	@staticmethod
	def _crc8Next(crc8, input):
		input ^= crc8
		for _ in range(0, 8):
			msb = input & 0x80
			input <<= 1
			if msb != 0:
				input ^= 0x07

		return input
