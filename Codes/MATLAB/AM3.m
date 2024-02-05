clear;clc;close all;


% Read wav file
filename = 'Ziyu Tian.wav';
[y, Fs] = audioread(filename);

% Calculate soundtrack duation time
duration = length(y) / Fs;

fs = 96000; % sampling frequency 
fp1 = 16000; % lower cutting frequency 
fp2 = 24000; % upper cutting frequency 

% Time Domain
time = linspace(0, duration, length(y));
figure;
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

% Hamming Windows applied 
window = hamming(N);
y_windowed = y_padded .* window;

Y_windowed = fft(y_padded .* window);



[b, a] = butter(2, [fp1, fp2]/(fs/2), 'bandpass');
%b = [0.0000273 0 -0.00005461 0 0.0000273];  % numerator coefficients for IIR filter
%a = [1.0077 -0.77 1.455 -0.598 0.5776];  % denominator coefficients for IIR filter

Y_filtered = filter(b, a, y_windowed);
amplitude_windowed = abs(Y_filtered) / N;
amplitude_windowed_dB = 20 * log10((amplitude_windowed));

frequencies_filtered = linspace(0, Fs, N) /1000;
amplitude_filtered = 20 * log10(abs(Y_filtered) / N);

plot(frequencies_filtered(1:N/2), amplitude_filtered(1:N/2),'b');
hold on;
plot(frequencies(1:N/2), amplitude_windowed_dB(1:N/2),'r');

title('Filtered Spectrum');
xlabel('Frequency (kHz)');
ylabel('Amplitude');
legend('Filtered','Original')