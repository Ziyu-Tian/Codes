filename = 'Ziyu Tian.wav';
[y, Fs] = audioread(filename);

duration = length(y) / Fs;

time = linspace(0, duration, length(y));

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

%[b, a] = butter(m, [fc_low, fc_high] / (Fs / 2), 'bandpass');
b = [0.0000273 0 -0.00005461 0 0.0000273];  % numerator coefficients for IIR filter
a = [1.0077 -0.87 1.455 -0.598 0.4776];  % denominator coefficients for IIR filter


y_filtered = filter(b, a, y_windowed);

figure(1);
subplot(2, 1, 1);
plot(time, y_filtered,'r');
%hold on;
%plot(time, y_windowed,'b');
title('Time Domain - Filtered');
xlabel('Time (s)');
ylabel('Amplitude');
subplot(2, 1, 2);
plot(time, y_windowed,'b')
title('Time Domain - Original');
xlabel('Time (s)');
ylabel('Amplitude');

% ÆµÂÊÓòÍ¼Ïñ - ÂË²¨ºó
Y_filtered = fft(y_filtered);
amplitude_filtered = 20 * log10(abs(Y_filtered) / N);
figure(2);
subplot(2, 1, 1);
plot(frequencies_original(1:N/2), amplitude_filtered(1:N/2),'r');
title('Frequency Domain - Filtered');
xlabel('Frequency (Hz)');
ylabel('Amplitude');

subplot(2, 1, 2);
plot(frequencies_original(1:N/2), amplitude_original(1:N/2),'b');

title('Frequency Domain - Original');
xlabel('Frequency (Hz)');
ylabel('Amplitude');
