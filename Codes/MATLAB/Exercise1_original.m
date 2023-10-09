% Demonstration of the fundamentals of discrete time signal creation and
% analysis in MATLAB
% J Neasham, Newcastle University, 2020
clear all; %clear workspace variables
close all; %close previous plots
fs = 5e4; %sampling frequency
Ts = 1/fs; %sample period
ADCBits = 4; %ADC sample resolution
f1 = 1000; %frequency of 1st sinusoid
a1 = 1.0; %amplitude of 1st sinusoid
Tmax = 0.005; %duration of signal
t = 0:Ts:Tmax; %generate discrete time values (nTs)
%{
figure; 
plot(t); %plot
xlabel('n'); ylabel('nTs (s)'); 
title('Discrete time values');
grid on;
pause(1000);
%}

%----------------- discrete time signal generation (A-Dconversion)--------------------
s1 = a1*sin(2*pi*f1*t); %generate sinusoid (double precision floating point)
%{
figure(1); 
plot(t,s1,'-o'); %plot
xlabel('t'); 
ylabel('s1'); 
title('Discrete time sinusoid (f1)');
grid on
pause;
%}
s1ADC = floor(s1*(2^(ADCBits-1))); %quantise to ADC scale (assuming ADC input range is +/-1)


%-------------- analog signal reconstruction (D-A conversion) -------------
s1a = s1ADC/(2^(ADCBits-1)); %convert back to voltage (DAC)
Error = s1-s1a; %calculate error (quantisation noise)
figure; plot(t,s1a,'-o'); %plot
xlabel('t'); 
ylabel('Voltage'); 
title('Quantised discrete time sinusoid (DAC voltage)');
grid on
hold; 
plot(t,Error,'r')
legend('Signal','Error')
pause(1000);
%{
%------------ interpolate (simulate analog reconstruction filter) -----------
s1s = resample(s1a,1e6/fs,1); %interpolate to 1 MHz sampling rate
t2 = 0:1e-6:(length(s1s)-1)*1e-6; %1MHz sample times
figure; plot(t2,s1s); %plot
xlabel('t'); ylabel('Voltage'); title('Reconstructed analog signal');
grid on

%}