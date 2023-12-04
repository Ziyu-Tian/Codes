filename = 'Ziyu Tian.wav';
[y, Fs] = audioread(filename);

Fc = 20000;

N = length(y);

frequencies_original = linspace(0, Fs, N) / 1000;


% Hamming Windows applied 
window = hamming(N);
y_windowed = y .* window;

Y_original = fft(y_windowed);

amplitude_original = 20 * log10(abs(Y_original) / N);


% Butterworth
fc_low = 16000;  
fc_high = 24000;
m = 2;          

[b, a] = butter(m, [fc_low, fc_high] / (Fs / 2), 'bandpass');
%b = [0.0000273 0 -0.00005461 0 0.0000273];  % numerator coefficients for IIR filter
%a = [1.0077 -0.87 1.455 -0.598 0.4776];  % denominator coefficients for IIR filter


y_filtered = filter(b, a, y_windowed);

t = (0:length(y_filtered)-1)/Fs;

local_signal = cos(2*pi*Fc*t);

y_mix = y_filtered .* (local_signal.');

%Y_mix = fft(y_mix);
%amplitude_filtered = 20 * log10(abs(Y_mix) / N);

figure(1);
pspectrum(fft(y_mix),Fs);


