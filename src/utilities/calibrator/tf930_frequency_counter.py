import time

class Tf930FrequencyCounter:
	def __init__(self, serial):
		if serial is None:
			raise TypeError('serial')

		self._serial = serial
		self._serial.open()

		self._reset()
		self._configure()

	def _reset(self):
		self._serial.write("\n*RST\n")
		time.sleep(1)
		self._serial.reset_input_buffer()

	def _configure(self):
		self._device_id = self._send_query('*IDN?')
		self._send_command('Z1')
		self._send_command('A1')
		self._send_command('AC')
		self._send_command('ER')
		self._send_command('FI')
		self._send_command('F2')
		self._send_command('M3')

	def _send_query(self, command):
		self._send_command(command + "\n")
		return self._serial.readline().strip()

	def _send_command(self, command):
		self._serial.write(command + "\n")

	def get_measured_frequency_hz(self):
		self.start_measuring_frequency()
		return self.await_measured_frequency_hz()

	def start_measuring_frequency(self):
		self._send_command('N?')

	def await_measured_frequency_hz(self):
		return float(self._serial.readline().strip())
