import jsonpickle

from intelhex import IntelHex
from pathlib import Path
from struct import unpack

from calibration_results import CalibrationResults
from cluck2sesame_device import Cluck2SesameDevice
from cluck2sesame_nvm_settings import Cluck2SesameNvmSettings

class CalibrationResultsLoader:
	def __init__(self, results_path):
		if results_path is None:
			raise TypeError('results_path')

		if not isinstance(results_path, Path):
			results_path = Path(results_path)

		if not results_path.is_dir():
			raise ValueError('results_path', 'Calibration path needs to be a directory')

		self._results_path = results_path

	def load(self):
		calibration_points = []
		for point_file in self._results_path.glob('point.*.json'):
			calibration_points.append(jsonpickle.decode(point_file.read_text()))

		nvm_settings_hex = IntelHex(str(Path(self._results_path, 'nvm-settings.pre.hex')))
		nvm_settings = Cluck2SesameNvmSettings(
			Cluck2SesameDevice.NVM_SETTINGS_ADDRESS,
			CalibrationResultsLoader._bytes_to_words(nvm_settings_hex.tobinarray()))

		return CalibrationResults(nvm_settings, calibration_points)

	@staticmethod
	def _bytes_to_words(bytes):
		return [word for word in unpack('<' + 'H' * (len(bytes) // 2), bytes)]
