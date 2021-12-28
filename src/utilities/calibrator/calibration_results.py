from cluck2sesame_nvm_settings import Cluck2SesameNvmSettings

class CalibrationResults:
	def __init__(self, nvm_settings, calibration_points):
		if nvm_settings is None:
			raise TypeError('nvm_settings')

		if calibration_points is None:
			raise TypeError('calibration_points')

		self._calibration_points = sorted(
			calibration_points,
			key=lambda point: (point.vdd_volts, point.temperature_celsius))

		# TODO:
		# find largest temperature differential between two identical VDDs then calculate delta degC and 8x ADC reading:
		self._highest_temperature_celsius = 34.5
		self._highest_temperature_adc = 8000

		# TODO: NEED TO CALCULATE THE TURNOVER TEMPERATURE ASSUMING 32768Hz AND TAKE ANOTHER READING AT THAT TEMPERATURE SO
		# THE STATIC FREQUENCY OFFSET AND PARABOLA COEFFICIENT CAN BE CALCULATED PROPERLY...

		self._nvm_settings = nvm_settings
		self._calibrated_nvm_settings = self._calculate_nvm_settings()

	def _calculate_nvm_settings(self):
		return self._nvm_settings.with_temperature_adc_high(self._highest_temperature_adc)

	@property
	def calibrated_nvm_settings(self):
		return self._calibrated_nvm_settings
