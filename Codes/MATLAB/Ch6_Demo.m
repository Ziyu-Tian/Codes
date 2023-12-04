% Demonstration of IIR filter implementation
% in MATLAB
% J Neasham, Newcastle University, 2020

clc;
close all;
clear all;

fs = 96000;
Ts = 1/fs;
fp1 = 16000;
fp2 = 24000;
%fc = 16000;
%Fc = fc/fs;
m = 2;

%[b, a] = butter(m, fc/(fs/2));
%[b, a] = butter(m, [fp1, fp2]/(fs/2), 'bandpass');

b = [0.0000273 0 -0.00005461 0 0.0000273];  % numerator coefficients for IIR filter
a = [1.0077 -0.77 1.455 -0.598 0.5776];  % denominator coefficients for IIR filter
%b = [0.20628 0 -2.51386 0 1.25693];
%a = [1 -0.69832 0.50681 -0.22469 0.19599];

x = randn(1, 5000) * sqrt(512);

xf_IIR = filter(b, a, x);


figure;
pspectrum(xf_IIR, fs);
ylabel('Power/Frequency (dB/Hz)');
title('IIR Filter Power Spectral Density');

pulse = [zeros(1, 10) ones(1, 30) zeros(1, 60)];

pulse_IIR = filter(b, a, pulse);

figure;
plot(pulse, 'k');
hold on;
plot(pulse_IIR, 'r');
xlim([1 100])
xlabel('n');
legend('Input pulse', 'IIR filter output');
title('IIR Filter Pulse Response');
