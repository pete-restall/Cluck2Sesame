import time

from cluck2sesame_parameters_sample import Cluck2SesameParametersSample
from cluck2sesame_nvm_settings import Cluck2SesameNvmSettings
from pic16f15356_device_information_area import Pic16f15356DeviceInformationArea

class Cluck2SesameDevice:
	def __init__(self, serial_factory):
		if serial_factory is None:
			raise TypeError('serial_factory')

		self._serial_factory = serial_factory
		self._serial = None

	def __enter__(self):
		self._serial = self._serial_factory()
		if self._serial is None:
			raise TypeError('serial')

		self._serial = self._serial.__enter__()
		if self._serial is None:
			raise TypeError('serial.__enter__')

		self._reset()
		return self

	def _reset(self):
		self._serial.write(b"\n")
		time.sleep(1)
		self._serial.reset_input_buffer()

	def __exit__(self, *args):
		if self._serial is not None:
			self._serial.__exit__()
			self._serial = None

	def refclk_on(self):
		return self._send_command('C1') == '1'

	def _send_command(self, command):
		if self._serial is None:
			raise Exception('Attempted to use the device outside of a "with" block or after disposal')

		self._serial.write(bytearray(command + "\n", encoding='ascii'))
		echo = str(self._serial.readline(), encoding='ascii').strip()
		if echo != command:
			raise Exception('Echo expected but corrupted; ' + command + ' --> ' + echo)

		result = str(self._serial.readline(), encoding='ascii').strip()
		if not result.startswith('='):
			raise Exception('Unsuccessful command; ' + command + ' --> ' + result)

		return result.lstrip('=')

	def refclk_off(self):
		return self._send_command('C0') == '0'

	def sample_parameters(self):
		return Cluck2SesameParametersSample(self._send_command('S'))

	def read_device_information_area(self):
		return Pic16f15356DeviceInformationArea(0x8100, self._read_nvm_at(range(0x8100, 0x8120)))

	def _read_nvm_at(self, addr_range):
		if not isinstance(addr_range, range):
			addr_range = range(addr_range, addr_range + 1)

		return [int(self._send_command('R{:04x}'.format(addr)), base=16) for addr in addr_range]

	def read_nvm_settings(self):
		return Cluck2SesameNvmSettings(0x3f80, self._read_nvm_at(range(0x3f80, 0x3fa0)))
