class Pic16f15356DeviceInformationArea:
	def __init__(self, address, dia):
		if len(dia) != 32:
			raise ValueError('dia', 'PIC16F15356 DIA is 32 words but received ' + len(dia) + ' words')

		self._address = address
		self._raw = dia.copy()
		self._device_id = ''.join(['{:04x}'.format(id_byte) for id_byte in dia[0x00:0x09]])
		self._fvra2x = dia[0x19]

	@property
	def address(self): return self._address

	@property
	def raw(self): return self._raw.copy()

	@property
	def device_id(self): return self._device_id

	@property
	def fvra2x(self): return self._fvra2x
