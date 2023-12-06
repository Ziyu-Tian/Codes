
clear all;

filename = 'Ziyu Tian.wav';
[y, Fs] = audioread(filename);

fs = 96000; % sampling frequency 
fp1 = 16000; % lower cutting frequency 
fp2 = 24000; % upper cutting frequency 

duration = length(y) / Fs;
time = linspace(0, duration, length(y));

n = length(y); % Use length of sound as FFT points, which meet the requirement of frequency resolution
N = pow2(nextpow2(n));

% Hamming Windows applied 
window = hamming(N);
y1 = [y; zeros(N - n, 1)];
y1 = y1 .* window;

frequencies = linspace(0, Fs, N);
frequencies = frequencies / 1000; % Change unit to kHz

m = 2;

[b1, a1] = butter(m, [fp1, fp2]/(fs/2), 'bandpass');

b = [0.0000273 0 -0.00005461 0 0.0000273];  % numerator coefficients for IIR filter
a = [1.0077 -0.77 1.455 -0.598 0.5776];  % denominator coefficients for IIR filter

% Frequency Response Testing

x = randn(1, 262144) * sqrt(512);
x = x .* window';

xf_IIR = filter(b, a, x);

xf_IIR_function = filter(b1,a1,x);

figure(1);
pspectrum(xf_IIR_function,Fs);
hold on;
pspectrum(x,Fs);


% Implmentation in wav file

%{
xf_IIR = filter(b, a, y);

xf_IIR_function = filter(b1,a1,y);

X1 = fft(xf_IIR,N);
X2 = fft(xf_IIR_function,N);
X0 = fft(y1,N);

Amplitude1 = 20*log10(abs(X1)/N);
Amplitude2 = 20*log10(abs(X2)/N);
Amplitude0 = 20*log10(abs(X0)/N);

figure(1);
plot(frequencies(1:N/2), Amplitude1(1:N/2),'b');
hold on;
plot(frequencies(1:N/2), Amplitude0(1:N/2),'r');


figure(2);
plot(frequencies(1:N/2), Amplitude2(1:N/2),'b');
hold on;
plot(frequencies(1:N/2), Amplitude0(1:N/2),'r');

figure(3);
plot(time, y,'b');
title('Time Domain');
xlabel('Time (s)');
ylabel('Amplitude');

hold on;
plot(time,xf_IIR_function,'r');

legend('Original','Filtered');

%}