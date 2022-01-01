class Parabola:
	def __init__(self, points):
		if len(points) != 3:
			raise Exception('Expected three points')

		x = [points[0].x, points[1].x, points[2].x]
		y = [points[0].y, points[1].y, points[2].y]

		denominator = (x[0] - x[1]) * (x[0] - x[2]) * (x[1] - x[2])
		self._coefficients = [
			(x[2] * (y[1] - y[0]) +
			x[1] * (y[0] - y[2]) +
			x[0] * (y[2] - y[1])) /
			denominator,

			(x[2] * x[2] * (y[0] - y[1]) +
			x[1] * x[1] * (y[2] - y[0]) +
			x[0] * x[0] * (y[1] - y[2])) /
			denominator,

			(x[1] * x[2] * (x[1] - x[2]) * y[0] +
			x[2] * x[0] * (x[2] - x[0]) * y[1] +
			x[0] * x[1] * (x[0] - x[1]) * y[2]) /
			denominator]

	@property
	def coefficients(self): return self._coefficients
