from types import SimpleNamespace

class Cluck2SesameParametersSample:
	def __init__(self, result):
		fields = result.split(',')
		if len(fields) != 4:
			raise Exception('Invalid parameters sample result from device; result=' + result)

		self._timestamp = int(fields[0], base=16)
		self._flags = int(fields[1], base=16)
		self._fvr_adc_aggregate = int(fields[2], base=16)
		self._temperature_adc_aggregate = int(fields[3], base=16)

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
