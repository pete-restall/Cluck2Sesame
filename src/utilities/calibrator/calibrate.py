import sys
import pyvisa

from pathlib import Path
from serial import Serial

from calibration_log import CalibrationLog
from calibration_process import CalibrationProcess
from calibration_results_loader import CalibrationResultsLoader
from calibrator import Calibrator
from cluck2sesame_device import Cluck2SesameDevice
from sdm3065x_frequency_counter import Sdm3065xFrequencyCounter
from tf930_frequency_counter import Tf930FrequencyCounter

calibration_dir = Path('../calibrated')

def calibrate_devices():
#	with Serial('/dev/ttyWHATEVER', baudrate=115200, timeout=30, xonxoff=True) as frequency_counter_port:
#		frequency_counter = Tf930FrequencyCounter(frequency_counter_port)

	global calibration_dir
	resources = pyvisa.ResourceManager('@py')
	with resources.open_resource('USB0::62700::60984::SDM36GBQ5R0755::0::INSTR') as resource:
		frequency_counter = Sdm3065xFrequencyCounter(resource)
		calibrator = CalibrationProcess(
			lambda: Calibrator(
				CalibrationLog(calibration_dir),
				frequency_counter,
				lambda: Cluck2SesameDevice(lambda: Serial('/dev/ttyS0', baudrate=9600, timeout=30)),
				vdd_volts_range=[2.7, 4.5]))

		calibrator.calibrate()

def recalculate_results_for(device_id):
	global calibration_dir
	loader = CalibrationResultsLoader(Path(calibration_dir, device_id))
	results = loader.load()
	log = CalibrationLog(calibration_dir).for_device(device_id)
	log.add_calibration_results(results)
	log.add_calibrated_nvm_settings(results.calibrated_nvm_settings)

if __name__ == '__main__':
	if len(sys.argv) == 1:
		calibrate_devices()
	else:
		recalculate_results_for(sys.argv[1])
