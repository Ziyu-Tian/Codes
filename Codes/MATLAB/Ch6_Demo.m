% Demonstration of IIR filter implementation
% in MATLAB
% J Neasham, Newcastle University, 2020
clc;
close all;
clear all;
fs = 1000; %sampling frequency
Ts = 1/fs; %sample period
fc = 100; %cut off frequency in Hz
Fc = fc/fs; %normalised cut off frequency
m = 10; %number of FIR taps (N = 2m+1)
N = 2*m+1; % total number of FIR filter taps
for n = 1:m
h(n) = 2*Fc*sin(n*2*pi*Fc)/(n*2*pi*Fc); %calculate truncated impulse response for LP filter (+ve n)
%h(n) = -2*Fc*sin(n*2*pi*Fc)/(n*2*pi*Fc); %calculate truncated impulse response for HP filter (+ve n)
end
h = [fliplr(h) 2*Fc h]; %construct filter (add n = 0 coefficient for LP and -vehalf)
x = randn(1,1000000)*sqrt(512); %generate white noise signal (normalise for default 512 point fft in pspectrum)
w = hamming(N)'; %generate N point hanning window
hw = h.*w; %apply window to filter coefficients
xf_FIR = conv(hw,x); %calculate filter output
figure;
pspectrum(xf_FIR,fs) % spectrum of output signal (frequency response)
ylabel('Gain (dB)');
hold;
legend('FIR filter');
title('Filter Frequency Response');
%b = [0.067 0.135 0.067]; %numerator coefficients for IIR filter
%a = [1 -1.1429 0.4127]; %denominator coefficients for IIR filter
%[b,a] = butter(m,fc/(fs/2)); %butterworth filter design (MATLAB function)
%xf_IIR = filter(b,a,x); %calculate IIR filter output
%pspectrum(xf_IIR,fs) % spectrum of output signal (frequency response)
legend('FIR','IIR');
pulse = [zeros(1,10) ones(1,30) zeros(1,60)]; %generate square pulse
pulse_FIR = conv(hw,pulse); %calculate FIR filter output
%pulse_IIR = filter(b,a,pulse); %calculate IIR filter output
figure;
plot(pulse,'k'); %plot filter outputs
hold;
plot(pulse_FIR,'b');
%plot(pulse_IIR,'r');
xlim([1 100])
xlabel('n');
legend('Input pulse','FIR filter output','IIR filter output');
title('pulse response');
