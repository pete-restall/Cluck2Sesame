from types import SimpleNamespace

from cluck2sesame_nvm_settings import Cluck2SesameNvmSettings
from parabola import Parabola

class CalibrationResults:
	def __init__(self, nvm_settings, calibration_points):
		if nvm_settings is None:
			raise TypeError('nvm_settings')

		if calibration_points is None:
			raise TypeError('calibration_points')

		if len(calibration_points) != 6:
			raise Exception('Expected 6 calibration points, high and low VDD over three temperatures')

		self._calibration_points = sorted(calibration_points, key=lambda point: point.vdd_volts)
		self._low_vdd_results = VddSpecificCalibrationResults(nvm_settings, self._calibration_points[0:3])
		self._high_vdd_results = VddSpecificCalibrationResults(nvm_settings, self._calibration_points[3:6])

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
	def calibrated_nvm_settings(self): return self._calibrated_nvm_settings

	@property
	def low_vdd(self): return self._low_vdd_results

	@property
	def high_vdd(self): return self._high_vdd_results

class VddSpecificCalibrationResults:
	def __init__(self, nvm_settings, calibration_points):
		if nvm_settings is None:
			raise TypeError('nvm_settings')

		if calibration_points is None:
			raise TypeError('calibration_points')

		if len(calibration_points) != 3:
			raise Exception('Expected three calibration points for three temperatures')

		self._calibration_points = sorted(
			calibration_points,
			key=lambda point: point.temperature_celsius)

		self._crystal_parabola = Parabola([
			VddSpecificCalibrationResults._point_to_xy(point) for point in self._calibration_points])

	@staticmethod
	def _point_to_xy(point):
		return SimpleNamespace(
			x=VddSpecificCalibrationResults._mean_temperature_adc(point.samples),
			y=point.clock_frequency_hz)

	@staticmethod
	def _mean_temperature_adc(samples):
		sum = 0
		for i in range(0, len(samples)):
			sum += samples[i].temperature_adc_mean

		return sum / len(samples)

	@property
	def calibration_points(self): return self._calibration_points.copy()

	@property
	def crystal_parabola(self): return self._crystal_parabola
