from statistics import mean

class CalibrationPoint:
	def __init__(self, samples, vdd_volts, temperature_celsius, clock_frequency_hz):
		if samples is None:
			raise TypeError('samples')

		if vdd_volts is None:
			raise TypeError('vdd_volts')

		if temperature_celsius is None:
			raise TypeError('temperature_celsius')

		if clock_frequency_hz is None:
			raise TypeError('clock_frequency_hz')

		self._samples = samples.copy()
		self._vdd_volts = vdd_volts
		self._temperature_adc = int(mean([sample.temperature_adc_mean for sample in self._samples]) + 0.5)
		self._temperature_celsius = temperature_celsius
		self._clock_frequency_hz = clock_frequency_hz

	@property
	def samples(self): return self._samples.copy()

	@property
	def vdd_volts(self): return self._vdd_volts

	@property
	def temperature_adc(self): return self._temperature_adc

	@property
	def temperature_celsius(self): return self._temperature_celsius

	@property
	def clock_frequency_hz(self): return self._clock_frequency_hz
