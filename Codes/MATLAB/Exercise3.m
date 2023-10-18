% Demonstration of FIR filter design by DFT method
% in MATLAB
% J Neasham, Newcastle University, 2020
clc;
close all;
clear all;
fs = 1000; %sampling frequency
Ts = 1/fs; %sample period
fc = 100; %cut off frequency in Hz
Fc = fc/fs; %normalised cut off frequency
m = 50; %number of taps (N = 2m+1)
N = 2*m+1; % total number of filter taps
for n = 1:m
    h(n) = 2*Fc*sin(n*2*pi*Fc)/(n*2*pi*Fc); %calculate truncated impulse response
for LP filter (+ve n)
%h(n) = -2*Fc*sin(n*2*pi*Fc)/(n*2*pi*Fc); %calculate truncated impulse response
for HP filter (+ve n)
end
plot(h)
pause;
h = [fliplr(h) 2*Fc h]; %construct filter (add n = 0 coefficient for LP and -ve
half)
%h = [fliplr(h) 1-2*Fc h]; %construct filter (add n = 0 coefficient for HP and
-ve half)
figure;
plot(h)
title('Filter coefficients');
pause;
x = randn(1,1000000)*sqrt(512); %generate white noise signal (normalise for
default 512 point fft in pspectrum)
xf = conv(h,x); %calculate filter output
w = hamming(N)'; %generate N point hanning window
hw = h.*w; %apply window to filter coefficients
figure;
plot(hw)
title('Windowed Filter coefficients');
pause;
xfw = conv(hw,x); %calculate filter output
figure;
pspectrum(xf,fs) % spectrum of output signal (frequency response)
ylabel('Gain (dB)');
hold;
pspectrum(xfw,fs) % spectrum of output signal (frequency response)
legend('Rectangular window','Hamming window');
title('Filter Frequency Response');
