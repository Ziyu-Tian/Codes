
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





[b,a] = butter(2,[fp1,fp2]/(fs/2),'bandpass');   % Butterworth IIR Bandpass 

Order = 4;

%x = y1;     

x = randn(1, 262144) * sqrt(512); % Generate a Random Noises with same sampling points of AM signal (N = 262144)
x = x .* window';

delay_x = zeros(Order+1,1);      % Create Delay Array for x and intialize with zeros (input)
delay_y = zeros(Order,1);        % Create Delay Array for x and intialize with zeros (output)

for i=1:length(x)
    for n=Order+1:-1:2          
        delay_x(n) = delay_x(n-1);      % Right-shift the delay_x Array 
    end
    delay_x(1) = x(i);              % Input each sample from signal x to the begining of delay_x

    Accumulator = 0;                    % Clear the Accumulator until the end of x-signal
    for n=1:Order+1                     % Calculate the result from input x                     
        Accumulator = Accumulator + b(n)*delay_x(n);    % + b(n)*x(i-n)
    end
    
    for n=2:Order+1                       % Calculate the result from output y                    
        Accumulator = Accumulator - a(n)*delay_y(n-1);    % -a(n)*y(i-n-1)
    end
    pulse_IIR2(i) = Accumulator;        % Store the output data of this sample x(i)
    for n=Order:-1:2          
        delay_y(n) = delay_y(n-1);      % Right-shift the delay_y Array 
    end
    delay_y(1) = Accumulator;           % Input each sample from signal x to the begining of delay_x
end

figure(2);
%pspectrum(pulse_IIR2(1:1780),fs)            % Output frequency spectrum 
pspectrum(pulse_IIR2(1:262144),fs) 

hold on;
pspectrum(x,fs) 
legend('After IIR Bandpass','Orignial');
title('IIR BP Frequency response');

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