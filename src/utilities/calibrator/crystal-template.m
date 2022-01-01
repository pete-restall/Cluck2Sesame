coefficients = [{coefficients}];

figure;
T = [0, 1023]; # Calibration points [{temperatures_adc}]
Trange = linspace(T(1), T(2));
title('Frequency (Hz) vs Temperature (ADC)');
plot(
	Trange,
	(coefficients(1) .* (Trange .^ 2)) .+ (coefficients(2) .* Trange) .+ coefficients(3),
	"-m;Computed;");

figure;
T = [{temperatures_celsius}];
Trange = linspace(T(1), T(3), 1000);
title('Frequency (Hz) vs Temperature (Celsius)');
plot(
	Trange,
	(coefficients(1) .* (Trange .^ 2)) .+ (coefficients(2) .* Trange) .+ coefficients(3),
	"-m;Computed;");

pause;
