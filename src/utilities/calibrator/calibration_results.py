from statistics import mean
from types import SimpleNamespace

from calibration_point import CalibrationPoint
from cluck2sesame_parameters_sample import Cluck2SesameParametersSample
from parabola import Parabola

class CalibrationResults:
	def __init__(self, nvm_settings, calibration_points):
		if nvm_settings is None:
			raise TypeError('nvm_settings')

		if calibration_points is None:
			raise TypeError('calibration_points')

		if len(calibration_points) != 6:
			raise Exception('Expected 6 calibration points, high and low VDD over three temperatures')

		self._calibration_points = sorted(calibration_points, key=lambda point: (point.vdd_volts, point.temperature_celsius))
		self._low_vdd = VddSpecificCalibrationResults(self._calibration_points[0:3])
		self._high_vdd = VddSpecificCalibrationResults(self._calibration_points[3:6])
		self._mean_vdd = VddSpecificCalibrationResults([
			CalibrationPoint(
				[Cluck2SesameParametersSample(
					timestamp=0x00,
					flags=0x00,
					fvr_adc_aggregate=(
						sum(map(lambda point: point.fvr_adc_aggregate, self._calibration_points[i].samples)) +
						sum(map(lambda point: point.fvr_adc_aggregate, self._calibration_points[i + 3].samples))
					) / (len(self._calibration_points[i].samples) + len(self._calibration_points[i + 3].samples)) + 0.5,
					temperature_adc_aggregate=(
						sum(map(lambda point: point.temperature_adc_aggregate, self._calibration_points[i].samples)) +
						sum(map(lambda point: point.temperature_adc_aggregate, self._calibration_points[i + 3].samples))
					) / (len(self._calibration_points[i].samples) + len(self._calibration_points[i + 3].samples)) + 0.5
				)],
				mean([self._calibration_points[i].vdd_volts, self._calibration_points[i + 3].vdd_volts]),
				mean([self._calibration_points[i].temperature_celsius, self._calibration_points[i + 3].temperature_celsius]),
				mean([self._calibration_points[i].clock_frequency_hz, self._calibration_points[i + 3].clock_frequency_hz]))
			for i in range(0, 3)
		])

		self._nvm_settings = nvm_settings
		self._calibrated_nvm_settings = self._calculate_nvm_settings()

	def _calculate_nvm_settings(self):
		return self._nvm_settings \
			.with_temperature_adc_high(self._mean_vdd.highest_temperature_calibration_point.temperature_adc) \
			.with_temperature_celsius_high(self._mean_vdd.highest_temperature_calibration_point.temperature_celsius) \
			.with_temperature_coefficient(self._mean_vdd.temperature_coefficient) \
			.without_calibration_required_flag() \
			.with_recalculated_crc8()

	@property
	def calibrated_nvm_settings(self): return self._calibrated_nvm_settings

	@property
	def low_vdd(self): return self._low_vdd

	@property
	def high_vdd(self): return self._high_vdd

	@property
	def mean_vdd(self): return self._mean_vdd

class VddSpecificCalibrationResults:
	def __init__(self, calibration_points):
		if calibration_points is None:
			raise TypeError('calibration_points')

		if len(calibration_points) != 3:
			raise Exception('Expected three calibration points for three temperatures')

		self._calibration_points = sorted(
			calibration_points,
			key=lambda point: point.temperature_celsius)

		self._crystal_parabola = Parabola([
			VddSpecificCalibrationResults._point_to_xy(point) for point in self._calibration_points])

		self._lowest_temperature_calibration_point = min(self._calibration_points, key=lambda point: point.temperature_celsius)
		self._highest_temperature_calibration_point = max(self._calibration_points, key=lambda point: point.temperature_celsius)
		self._temperature_coefficient = \
			(self._highest_temperature_calibration_point.temperature_celsius - self._lowest_temperature_calibration_point.temperature_celsius) / \
			(self._highest_temperature_calibration_point.temperature_adc - self._lowest_temperature_calibration_point.temperature_adc)

		self._vdd_volts = calibration_points[0].vdd_volts

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

	@property
	def highest_temperature_calibration_point(self): return self._highest_temperature_calibration_point

	@property
	def lowest_temperature_calibration_point(self): return self._lowest_temperature_calibration_point

	@property
	def temperature_coefficient(self): return self._temperature_coefficient

	@property
	def vdd_volts(self): return self._vdd_volts
