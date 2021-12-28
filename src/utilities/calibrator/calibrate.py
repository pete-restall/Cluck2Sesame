import pyvisa

from calibration_process import CalibrationProcess
from calibration_log import CalibrationLog
from calibrator import Calibrator
from cluck2sesame_device import Cluck2SesameDevice
from serial import Serial

from tf930_frequency_counter import Tf930FrequencyCounter
from sdm3065x_frequency_counter import Sdm3065xFrequencyCounter

if __name__ == '__main__':
#	with Serial('/dev/ttyWHATEVER', baudrate=115200, timeout=30, xonxoff=True) as frequency_counter_port:
#		frequency_counter = Tf930FrequencyCounter(frequency_counter_port)

	resources = pyvisa.ResourceManager('@py')
	with resources.open_resource('USB0::62700::60984::SDM36GBQ5R0755::0::INSTR') as resource:
		frequency_counter = Sdm3065xFrequencyCounter(resource)
		calibrator = CalibrationProcess(
			lambda: Calibrator(
				CalibrationLog('../calibrated'),
				frequency_counter,
				lambda: Cluck2SesameDevice(lambda: Serial('/dev/ttyS0', baudrate=9600, timeout=30))))

		calibrator.calibrate()
