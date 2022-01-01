class CalibrationProcess:
	def __init__(self, calibrator_factory):
		if calibrator_factory is None:
			raise TypeError('calibrator_factory')

		self._calibrator_factory = calibrator_factory

	def calibrate(self):
		calibrate_another = True
		while calibrate_another:
			while True:
				ready = input('Is the device connected and ready for calibration; [y]es, [n]o or [q]uit ?  ')
				if ready in ('q', 'Q'):
					calibrate_another = False
					break
				elif ready in ('y', 'Y'):
					break
				else:
					print('Enter [y] to start device calibration, or [q] to quit.')

			if calibrate_another:
				calibrator = self._calibrator_factory()
				if calibrator is None:
					raise TypeError('calibrator')

				calibrator.calibrate()
