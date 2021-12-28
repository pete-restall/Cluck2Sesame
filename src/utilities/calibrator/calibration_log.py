import json

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
		file.write_text(json.dumps(
			calibration_point,
			default=lambda x: vars(x),
			sort_keys=False,
			indent=4))

	def add_calibration_results(self, results):
		file = self._file_path_for('results.json')
		file.write_text(json.dumps(
			results,
			default=lambda x: vars(x),
			sort_keys=False,
			indent=4))

		return self
