
clc;
close all;
clear all;

filename = 'Ziyu Tian.wav';
[y, Fs] = audioread(filename);

fs = 96000; % sampling frequency 
fp1 = 16000; % lower cutting frequency 
fp2 = 24000; % upper cutting frequency 

m = 2;

[b1, a1] = butter(m, [fp1, fp2]/(fs/2), 'bandpass');

%b = [0.0000273 0 -0.00005461 0 0.0000273];  % numerator coefficients for IIR filter
%a = [1.0077 -0.77 1.455 -0.598 0.5776];  % denominator coefficients for IIR filter


%x = randn(1, 5000) * sqrt(512);

%xf_IIR = filter(b, a, x);

xf_IIR_function = filter(b1,a1,y);

%{
figure(1);
pspectrum(xf_IIR, fs);
ylabel('Power/Frequency (dB/Hz)');
title('Indivisual Designed IIR Filter Power Spectral Density');
%}

figure(2);
pspectrum(xf_IIR_function, fs);
ylabel('Power/Frequency (dB/Hz)');
title('MATLAB Generated IIR Filter Power Spectral Density');

