import time

class Sdm3065xFrequencyCounter:
	def __init__(self, visa):
		if visa is None:
			raise TypeError('visa')

		self._visa = visa
		self._reset()
		self._configure()

	def _reset(self):
		self._send_command('*RST')

	def _send_command(self, command):
		if '?' in command:
			return self._visa.query(command, delay=0.01).strip()

		return self._visa.write(command)

	def _configure(self):
		self._device_id = self._send_command('*IDN?')
		self._send_command('CONFIGURE:FREQUENCY')
		self._send_command('SENSE:FREQUENCY:VOLTAGE:RANGE 2V')
		self._send_command('SENSE:FREQUENCY:APERTURE 1s')
		self._send_command('SAMPLE:COUNT 10')
		self._send_command('TRIGGER:SOURCE IMMEDIATE')

	def get_measured_frequency_hz(self):
		self.start_measuring_frequency()
		return self.await_measured_frequency_hz()

	def start_measuring_frequency(self):
		self._send_command('INITIATE')

	def await_measured_frequency_hz(self):
		while int(self._send_command('DATA:POINTS?')) < 10:
			time.sleep(0.1)

		return float(self._send_command('CALCULATE:AVERAGE:AVERAGE?'))
