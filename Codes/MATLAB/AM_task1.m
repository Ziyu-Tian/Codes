clear;clc;close all;


% Read wav file
filename = 'Ziyu Tian.wav';
[y, Fs] = audioread(filename);

% Calculate soundtrack duation time
duration = length(y) / Fs;


% Time Domain
time = linspace(0, duration, length(y));
figure;
subplot(2, 1, 1);
plot(time, y);
title('Time Domain');
xlabel('Time (s)');
ylabel('Amplitude');


% Frequency Domain

n = length(y); % Use length of sound as FFT points, which meet the requirement of frequency resolution
N = pow2(nextpow2(n));

frequencies = linspace(0, Fs, N);
frequencies = frequencies / 1000; % Change unit to kHz
y_padded = [y; zeros(N - n, 1)];  % Zero-padding to match N
Y = fft(y_padded);

% Amplitude Normalization 
amplitude = abs(Y)/N; 
% Amplotude Scaling to dB
amplitude_dB = 20 * log10(amplitude); 
subplot(2, 1, 2);
% According to Nyquist Limit, a half frequencies are enough
plot(frequencies(1:N/2), amplitude_dB(1:N/2),'b'); 
hold on;
% Hamming Windows applied 
window = hamming(N);
Y_windowed = fft(y_padded .* window);
amplitude_windowed = abs(Y_windowed) / N;
amplitude_windowed_dB = 20 * log10(amplitude_windowed);
plot(frequencies(1:N/2), amplitude_windowed_dB(1:N/2),'r');
title('Frequency Domain');
xlabel('Frequency(kHz)');
ylabel('Amplitude (dB)');
legend('Original Signal', 'Hamming Processed Signal');
