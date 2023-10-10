% Demonstration of DFT(FFT) and spectral leakage
% analysis in MATLAB
% J Neasham, Newcastle University, 2020
clear; %clear workspace variables
close all; %close previous plots
j = sqrt(-1); %for complex numbers
fs = 32; %sampling frequency
Ts = 1/fs; %sample period
f1 = 8; %frequency of 1st sinusoid
f2 =10;
a1 = 1.0;
a2 = 1.0; %amplitude of 1st sinusoid
N = 64; %no of samples in transform
t = 0:Ts:(N-1)*Ts; %generate discrete time values (nTs)

%----------------- discrete time signal generation --------------------
s3 = a1*cos(2*pi*f1*t) + a2*cos(2*pi*f2*t); %generate sinusoid (double precision floating point)
%s1 = cos(2*pi*f1*t)+j*sin(2*pi*f1*t); %exp(j*2*pi*f1*t)
%{
figure; 
plot(t,s1); %plot
xlabel('t'); ylabel('s1'); title('Discrete time sinusoid (f1)');
grid on
%}
%------------ calculate Fast Fourier Transform (FFT) ---------------
s3_fft = fft(s3);
k = 0:1:N-1;
figure(1);
subplot(3,1,1); 
plot(k,real(s3_fft)); %plot real part of FFT coefficients
ylim([-N/2-1 N/2+1]); % set y axis limits
ylabel('Real');
title('FFT Coefficients');
subplot(3,1,2); 
plot(k,imag(s3_fft)); %plot imaginary part of FFT coefficients
ylim([-N/2-1 N/2+1]); % set y axis limits
ylabel('Imaginary');
subplot(3,1,3); 
plot(k,abs(s3_fft)); %plot magnitude of FFT coefficients
ylim([0 N/2+1]); % set y axis limits
xlabel('Discrete Frequency (k)'); 
ylabel('Magnitude');

%------------ calculate FFT with hamming window ---------------
h = 0.5*(1-cos(2*pi*k/(N-1))); %generate N point hanning window
s3h = s3.*h; %apply window to signal
figure(2); 
plot(t,s3h); %plot
xlabel('t'); ylabel('s1h'); title('Windowed sine wave');
grid on
s3_fft_h = fft(s3h); % calculate windowed FFT
figure;
plot(k,abs(s3_fft)); %plot magnitude of FFT coefficients
ylim([0 N/2+1]); % set y axis limits
xlabel('Discrete Frequency (k)'); ylabel('Magnitude');
hold;
plot(k,abs(s3_fft_h),'r'); %plot magnitude of windowed FFT coefficients
title('Windowed FFT Coefficients');
legend('Rectangular window','Hanning window');
pause(1000);