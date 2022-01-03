global crystalCoefficients = [{crystal_coefficients}];
global temperatureHighCelsius = {temperature_high_celsius};
global temperatureHighAdcCount = {temperature_high_adc};
global temperatureCoefficient = {temperature_coefficient};
global calibrationPoints = [{temperatures_adc}];
global vddVolts = {vdd_volts};

function adcCount = celsiusToAdcCount(celsius)
	global temperatureHighCelsius;
	global temperatureHighAdcCount;
	global temperatureCoefficient;
	adcCount = round(temperatureHighAdcCount + (celsius - temperatureHighCelsius) / -temperatureCoefficient);
endfunction

function frequencyHz = adcCountToFrequencyHz(adcCount)
	global crystalCoefficients;
	if isvector(adcCount)
		frequencyHz = (crystalCoefficients(1) .* (adcCount .^ 2)) .+ (crystalCoefficients(2) .* adcCount) .+ crystalCoefficients(3);
	else
		frequencyHz = (crystalCoefficients(1) * (adcCount ^ 2)) + (crystalCoefficients(2) * adcCount) + crystalCoefficients(3);
	endif
endfunction

figure;
T = [0, 1023];
Trange = linspace(T(1), T(2));
title('Absolute Frequency (Hz) vs Temperature (ADC)');
plot(Trange, adcCountToFrequencyHz(Trange), "-m;Computed;");

figure;
title('Delta Frequency (Hz) vs Temperature (ADC)');
plot(Trange, 32768 - adcCountToFrequencyHz(Trange), "-m;Computed;");

figure;
T = [-20, 40];
Trange = linspace(T(1), T(2), 1000);
TrangeAdc = celsiusToAdcCount(Trange);
title('Absolute Frequency (Hz) vs Temperature (Celsius)');
plot(Trange, adcCountToFrequencyHz(TrangeAdc), "-m;Computed;");

figure;
title('Delta Frequency (Hz) vs Temperature (Celsius)');
plot(Trange, 32768 - adcCountToFrequencyHz(TrangeAdc), "-m;Computed;");

pause;
