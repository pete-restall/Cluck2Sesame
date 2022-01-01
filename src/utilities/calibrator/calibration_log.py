import jsonpickle

from pathlib import Path

from intelhex import IntelHex

class CalibrationLog:
	def __init__(self, path):
		if path is None:
			raise TypeError('path')

		if not isinstance(path, Path):
			path = Path(path)

		if not path.is_dir():
			raise ValueError('path', 'Calibration path needs to be a directory')

		self._path = path

	def for_device(self, device_id):
		path = Path(self._path, device_id)
		return DeviceCalibrationLog(path)

class DeviceCalibrationLog:
	def __init__(self, path):
		if path is None:
			raise TypeError('path')

		if not isinstance(path, Path):
			path = Path(path)

		path.mkdir(exist_ok=True)
		if not path.is_dir():
			raise ValueError('path', 'Calibration path needs to be a directory')

		self._path = path

	def add_device_information_area(self, dia):
		self._write_raw_words_as_hex('dia.hex', dia)
		return self

	def _write_raw_words_as_hex(self, filename, section, high_mask=0x3f):
		hex_file = IntelHex()
		addr = section.address * 2
		for word in section.raw:
			hex_file[addr + 0] = (word >> 0) & 0xff
			hex_file[addr + 1] = (word >> 8) & high_mask
			addr += 2

		hex_file.write_hex_file(self._file_path_for(filename))

	def _file_path_for(self, filename):
		return Path(self._path, filename)

	def add_initial_nvm_settings(self, nvm_settings):
		self._write_raw_words_as_hex('nvm-settings.pre.hex', nvm_settings)
		return self

	def add_calibrated_nvm_settings(self, nvm_settings):
		self._write_raw_words_as_hex('nvm-settings.post.hex', nvm_settings)
		return self

	def add_calibration_points(self, calibration_points):
		for i in range(0, len(calibration_points)):
			self._add_calibration_point(self._file_path_for(f'point.{i}.json'), calibration_points[i])

		return self

	def _add_calibration_point(self, file, calibration_point):
		file.write_text(jsonpickle.encode(calibration_point, indent=4))

	def add_calibration_results(self, results):
		file = self._file_path_for('results.json')
		file.write_text(jsonpickle.encode(results, indent=4))
		return self._add_crystal_parabolas(results)

	def _add_crystal_parabolas(self, results):
		template = Path('crystal-template.m').read_text()
		DeviceCalibrationLog._write_crystal_parabola_octave(template, results.low_vdd, self._file_path_for('crystal-low-vdd.m'))
		DeviceCalibrationLog._write_crystal_parabola_octave(template, results.high_vdd, self._file_path_for('crystal-high-vdd.m'))
		return self

	@staticmethod
	def _write_crystal_parabola_octave(template, results, filename):
		(temperatures_adc, temperatures_celsius) = DeviceCalibrationLog._temperatures_from(results.calibration_points)
		coefficients = ', '.join([str(c) for c in results.crystal_parabola.coefficients])
		filename.write_text(template
			.replace('{coefficients}', coefficients)
			.replace('{temperatures_adc}', temperatures_adc)
			.replace('{temperatures_celsius}', temperatures_celsius)) # TODO: NEED TO SUBSTITUTE THE FVR / DIA VALUES AS WELL, SO FREQUENCY-VS-TEMPERATURE (AS OPPOSED TO FREQUENCY-VS-ADC) CAN BE PLOTTED

	@staticmethod
	def _temperatures_from(calibration_points):
		return (
			', '.join([str(point.temperature_adc) for point in calibration_points]),
			', '.join([str(point.temperature_celsius) for point in calibration_points]))

