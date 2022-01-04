from types import SimpleNamespace

class Cluck2SesameParametersSample:
	def __init__(self, timestamp, flags, fvr_adc_aggregate, temperature_adc_aggregate):
		if timestamp is None:
			raise TypeError('timestamp')

		if flags is None:
			raise TypeError('flags')

		if fvr_adc_aggregate is None:
			raise TypeError('fvr_adc_aggregate')

		if temperature_adc_aggregate is None:
			raise TypeError('temperature_adc_aggregate')

		self._timestamp = int(timestamp)
		if self._timestamp < 0 or self._timestamp > 59:
			raise Exception(f'Timestamp must be an integral number of seconds; timestamp={self._timestamp}')

		self._flags = int(flags)
		if self._flags < 0 or self._flags > 0xff:
			raise Exception(f'Flags must be a single byte; flags=0x{self._flags:2x}')

		self._fvr_adc_aggregate = int(fvr_adc_aggregate)
		if self._fvr_adc_aggregate < 0 or self._fvr_adc_aggregate > 8 * 1023:
			raise Exception(f'FVR ADC aggregate value must be a maximum of 8x unsigned 10-bit samples; value=0x{self._fvr_adc_aggregate:2x}')

		self._temperature_adc_aggregate = int(temperature_adc_aggregate)
		if self._temperature_adc_aggregate < 0 or self._temperature_adc_aggregate > 8 * 1023:
			raise Exception(f'Temperature ADC aggregate value must be a maximum of 8x unsigned 10-bit samples; value=0x{self._temperature_adc_aggregate:2x}')

	@classmethod
	def from_raw(cls, raw):
		fields = raw.split(',')
		if len(fields) != 4:
			raise Exception('Invalid parameters sample result from device; result=' + raw)

		return cls(
			int(fields[0], base=16),
			int(fields[1], base=16),
			int(fields[2], base=16),
			int(fields[3], base=16))

	@property
	def timestamp(self): return self._timestamp

	@property
	def flags(self): return Cluck2SesameParametersSample._decode_flags(self._flags)

	@property
	def fvr_adc_aggregate(self): return self._fvr_adc_aggregate

	@property
	def fvr_adc_mean(self): return self._fvr_adc_aggregate / 8

	@property
	def temperature_adc_aggregate(self): return self._temperature_adc_aggregate

	@property
	def temperature_adc_mean(self): return self._temperature_adc_aggregate / 8

	@staticmethod
	def _decode_flags(flags):
		as_obj = SimpleNamespace(raw=flags)
		if flags & 0b0000_0001:
			as_obj.is_vdd_regulated = True

		return as_obj
