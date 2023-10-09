
close all; %close previous plots
fs = 1000; %sampling frequency
Ts = 1/fs; %sample period
ADCBits = 8; %ADC sample resolution
f1 = 100; %frequency of 1st sinusoid
f2 = 150;
a1 = 1.0; %amplitude of 1st sinusoid
a2 = 1e-4;
Tmax = 1; %duration of signal
t = 0:Ts:Tmax; %generate discrete time values (nTs)

% discrete time signal generation (A-D£©
s1 = a1*sin(2*pi*f1*t);
s2 = a2*sin(2*pi*f2*t);
s3 = s1 + s2; 

s3ADC = floor(s3*(2^(ADCBits-1))); %quantise to ADC scale (assuming ADC input range is +/-1)
figure(2); 
%plot(t,s3ADC,'-o'); %plot
xlabel('t'); 
ylabel('ADC value'); 
title('Quantised discrete time sinusoid (ADC values)');
grid on
%pause(1000);

% analog signal reconstruction (D-A conversion)
s3a = s3ADC/(2^(ADCBits-1)); %convert back to voltage (DAC)
Error = s3-s3a; %calculate error (quantisation noise)
figure(3); 
%{
plot(t,s3a,'-o'); %plot
xlabel('t'); 
ylabel('Voltage'); 
title('Quantised discrete time sinusoid (DAC voltage)');
grid on
hold; 
plot(t,Error,'r')
legend('Signal','Error')
pause(1000);
%}

% 


%{
%------------ interpolate (simulate analog reconstruction filter) -----------
s1s = resample(s1a,1e6/fs,1); %interpolate to 1 MHz sampling rate
t2 = 0:1e-6:(length(s1s)-1)*1e-6; %1MHz sample times
figure; plot(t2,s1s); %plot
xlabel('t'); ylabel('Voltage'); title('Reconstructed analog signal');
grid on
%}
