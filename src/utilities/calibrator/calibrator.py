import time

from calibration_point import CalibrationPoint
from calibration_results import CalibrationResults

class Calibrator:
	def __init__(self, calibration_log, frequency_counter, device_factory, vdd_volts_range):
		if calibration_log is None:
			raise TypeError('calibration_log')

		if frequency_counter is None:
			raise TypeError('frequency_counter')

		if device_factory is None:
			raise TypeError('device_factory')

		if vdd_volts_range is None:
			raise TypeError('vdd_volts_range')

		if len(vdd_volts_range) != 2:
			raise Exception('VDD voltage range must be two voltages; one high, one low')

		vdd_volts_range = sorted(vdd_volts_range)
		if vdd_volts_range[0] < 2.7 or vdd_volts_range[1] > 4.5:
			raise Exception('VDD voltages are out of range for the device; 2.7V <= VDD <= 4.5V')

		if (vdd_volts_range[1] - vdd_volts_range[0]) < 1:
			raise Exception('VDD voltage range is too narrow')

		self._calibration_log = calibration_log
		self._frequency_counter = frequency_counter
		self._device_factory = device_factory
		self._vdd_volts_range = vdd_volts_range

	def calibrate(self):
		with self._device_factory() as device:
			dia = device.read_device_information_area()
			log = self._calibration_log.for_device(dia.device_id)
			log.add_device_information_area(dia)

			nvm_settings = device.read_nvm_settings()
			log.add_initial_nvm_settings(nvm_settings)

			device.refclk_on()

			calibration_points = [
				self._sample_point(device, self._vdd_volts_range[0]),
				self._sample_point(device, self._vdd_volts_range[1]),
				self._sample_point(device, self._vdd_volts_range[0]),
				self._sample_point(device, self._vdd_volts_range[1]),
				self._sample_point(device, self._vdd_volts_range[0]),
				self._sample_point(device, self._vdd_volts_range[1])
			]
			log.add_calibration_points(calibration_points)

			results = CalibrationResults(nvm_settings, calibration_points)

			log.add_calibration_results(results)
			log.add_calibrated_nvm_settings(results.calibrated_nvm_settings)

			device.refclk_off()

	def _sample_point(self, device, vdd_volts):
		Calibrator._confirm_vdd_is(vdd_volts)
		temperature_celsius = Calibrator._get_temperature()

		self._frequency_counter.start_measuring_frequency()

		samples = []
		for i in range(0, 10):
			sample = device.sample_parameters()
			if getattr(sample.flags, 'is_vdd_regulated', False):
				raise Exception('Sample taken with +3.3V regulated voltage - calibration depends on variable VDD !')

			samples.append(sample)
			print(
				f'TIMESTAMP={sample.timestamp:02}, ' +
				f'FLAGS=0x{sample.flags.raw:02x}, ' +
				f'FVR_adc={sample.fvr_adc_mean:f}, ' +
				f'TEMP_adc={sample.temperature_adc_mean:f}')

			time.sleep(1)

		clock_frequency_hz = self._frequency_counter.await_measured_frequency_hz()
		print(f'Clock Frequency = {clock_frequency_hz:f}Hz')

		return CalibrationPoint(samples, vdd_volts, temperature_celsius, clock_frequency_hz)

	@staticmethod
	def _confirm_vdd_is(volts):
		while True:
			confirmation = input(f'Is VDD set to {volts}V; [y]es or [n]o ?  ')
			if confirmation in ('y', 'Y'):
				return

			print(f'This calibration point requires a VDD of {volts}V.')

	@staticmethod
	def _get_temperature():
		while True:
			temperature_celsius = Calibrator._float_input('Enter Current Temperature (Celsius): ')
			if temperature_celsius < -10 or temperature_celsius > 40:
				print('Temperature is out of range for the device; -10C <= temp <= 40C')
			else:
				return temperature_celsius

	@staticmethod
	def _float_input(prompt):
		while True:
			value = input(prompt)
			try:
				return float(value)
			except ValueError:
				print('Value is not convertible to a float !')
