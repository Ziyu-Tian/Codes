clc;
close all;
clear all;

filename = 'Ziyu Tian.wav';
[y, Fs] = audioread(filename);

fs = 96000; % sampling frequency 
fp1 = 16000; % lower cutting frequency 
fp2 = 24000; % upper cutting frequency 

duration = length(y) / Fs;
n = length(y); % Use length of sound as FFT points, which meet the requirement of frequency resolution
N = pow2(nextpow2(n));


% Hamming Windows applied 
window = hamming(N);
y1 = [y; zeros(N - n, 1)];
y1 = y1 .* window;
t = linspace(0, duration, length(y1));


[b,a] = butter(2,[fp1,fp2]/(fs/2),'bandpass');   % Butterworth IIR Bandpass 

frequencies = linspace(0, Fs, N);
frequencies = frequencies / 1000; % Change unit to kHz



x = y1;     

%x = randn(1, 262144) * sqrt(512); % Generate a Random Noises with same sampling points of AM signal (N = 262144)
%x = x .* window';

Order = 4;
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
    BP_IIR(i) = Accumulator;        % Store the output data of this sample x(i)
    for n=Order:-1:2          
        delay_y(n) = delay_y(n-1);      % Right-shift the delay_y Array 
    end
    delay_y(1) = Accumulator;           % Input each sample from signal x to the begining of delay_x
end

%{
figure(1);
%pspectrum(BP_IIR(1:1780),fs)            % Output frequency spectrum 
pspectrum(BP_IIR(1:262144),fs) 

hold on;
pspectrum(x,fs) 
legend('After IIR Bandpass','Orignial');
title('IIR BP Frequency response');
xlabel('Time (s)');
ylabel('Amplitude');

figure(2);
plot(t,y1,'b');
hold on;
plot(t,BP_IIR,'r');
legend('Original','After BP Filtered');
title('Comparision in Time Domain');
xlabel('Time (s)');
ylabel('Amplitude');
%}


lo_frequency = 20000;  % Local oscillitor Frequency (same as Carrier Frequency, 20 kHz from previous analysis)
lo_signal1 = cos(2 * pi * lo_frequency * t)'; % Real Part of the AM signal
lo_signal2 = sin(2 * pi * lo_frequency * t)'; % Imaginary Part of the AM signal
mixed_signal = BP_IIR .* lo_signal1'; % Signal Mixing 
mixed_signal2 = BP_IIR.* lo_signal2';

%{

Fc = 4000/(Fs); % Normalized cutting frequency 
m = 264; 
N1 = 2*m+1; % N1 = 5.5 / (1000/96000) = 528 (529)

tic
h_single_stage = zeros(1, 2*m + 1); % Intialize the FIR array with zeros
for n = 1:m
    h_single_stage(n) = 2 * Fc * sin(n * 2 * pi * Fc ) / (n * 2 * pi * Fc); 
    % Truncate the Low-pass Impluse Response Fucntion with m coefficients (first half of the FIR array)
end

h_single_stage = [fliplr(h_single_stage(1:m)) 2*Fc h_single_stage(1:m)];
% Construct the fliped another half, the 2Fc component in the middle and the original half of FIR Coefficients Array

w = blackman(N1)'; % Applying the suitable window function 
h_single_stage = h_single_stage.*w;

% Single stage convolution 
x =mixed_signal;
%x = randn(1, 262144) * sqrt(512); % Generate a Random Noises with same sampling points of AM signal (N = 262144)

y_single_stage = zeros(size(x));
for i = 1:length(x)
    for j = 1:length(h_single_stage)
        if i - j + 1 > 0
            y_single_stage(i) = y_single_stage(i) + x(i - j + 1) * h_single_stage(j); % Convolution of h_FIR() and x()
        end
    end
end
toc
% Single stage convolution 
x2 =mixed_signal2;
%x = randn(1, 262144) * sqrt(512); % Generate a Random Noises with same sampling points of AM signal (N = 262144)

y_single_stage2 = zeros(size(x2));
for i = 1:length(x2)
    for j = 1:length(h_single_stage)
        if i - j + 1 > 0
            y_single_stage2(i) = y_single_stage2(i) + x2(i - j + 1) * h_single_stage(j); % Convolution of h_FIR() and x()
        end
    end
end

%}


%-------------------------------------------Stage-two Version-----------------------------------

% Single Stage FIR
Fc = 4000/(Fs/2); % transition width = 4 to 9 (5kHz), decimation factor = 2
m = 26; 
N1 = 2*m+1;
tic;
h_single_stage = zeros(1, 2*m + 1);
for n = 1:m
    h_single_stage(n) = 2 * Fc * sin(n * 2 * pi * Fc ) / (n * 2 * pi * Fc);
end

h_single_stage = [fliplr(h_single_stage(1:m)) 2 * Fc h_single_stage(1:m)];

w = blackman(N1)';
h_single_stage = h_single_stage.*w;

% Single stage convolution 
x =mixed_signal;

y_single_stage = zeros(size(x));
for i = 1:length(x)
    for j = 1:length(h_single_stage)
        if i - j + 1 > 0
            y_single_stage(i) = y_single_stage(i) + x(i - j + 1) * h_single_stage(j);
        end
    end
end

% Second Stage FIR
m2 = 66; % transition width = 4 to 5 (1kHz), decimation factor = 2
N2 = 2*m2+1;
Fc = 4000/(Fs/4);

h_two_stage = zeros(1, 2*m2 + 1);
for n = 1:m2
    h_two_stage(n) = 2 * Fc * sin(n * 2 * pi * Fc ) / (n * 2 * pi * Fc);
end

h_two_stage = [fliplr(h_two_stage(1:m2)) 2 * Fc h_two_stage(1:m2)];

w = blackman(N2)';
h_two_stage = h_two_stage.*w;

% Second Stage Convolution 
x = y_single_stage;

y_two_stage = zeros(size(x));
for i = 1:length(x)
    for j = 1:length(h_two_stage)
        if i - j + 1 > 0
            y_two_stage(i) = y_two_stage(i) + x(i - j + 1) * h_two_stage(j);
        end
    end
end
toc;
% Load calculation = 53 x 96000 x 0.5 + 133 x 96000 x 0.25 = 5.73 M
% Calculation Time = 0.274657s
%-------------------------------------------Stage-two Version End-----------------------------------

% Single stage convolution 
x1 =mixed_signal2;

y_single_stage = zeros(size(x1));
for i = 1:length(x1)
    for j = 1:length(h_single_stage)
        if i - j + 1 > 0
            y_single_stage(i) = y_single_stage(i) + x(i - j + 1) * h_single_stage(j);
        end
    end
end

% Second Stage FIR
m2 = 66; % transition width = 4 to 5 (1kHz), decimation factor = 2
N2 = 2*m2+1;
Fc = 4000/(Fs/4);

h_two_stage2 = zeros(1, 2*m2 + 1);
for n = 1:m2
    h_two_stage2(n) = 2 * Fc * sin(n * 2 * pi * Fc ) / (n * 2 * pi * Fc);
end

h_two_stage2 = [fliplr(h_two_stage2(1:m2)) 2 * Fc h_two_stage2(1:m2)];

w = blackman(N2)';
h_two_stage2 = h_two_stage2.*w;

% Second Stage Convolution 
x1 = y_single_stage;

y_two_stage2 = zeros(size(x1));
for i = 1:length(x1)
    for j = 1:length(h_two_stage2)
        if i - j + 1 > 0
            y_two_stage2(i) = y_two_stage2(i) + x(i - j + 1) * h_two_stage2(j);
        end
    end
end

magnitude_demodulated_signal = sqrt(y_two_stage.^2 + y_two_stage2.^2);


%{
fc1 = 20;
[b1,a1] = butter(2,fc1/(fs/2),'high');   % Butterworth IIR High-pass 

x = magnitude_demodulated_signal;     

Order = 4;
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
    HP_IIR(i) = Accumulator;        % Store the output data of this sample x(i)
    for n=Order:-1:2          
        delay_y(n) = delay_y(n-1);      % Right-shift the delay_y Array 
    end
    delay_y(1) = Accumulator;
end

%}


cutoff_frequency = 1; % Adjust as needed
[b_hp, a_hp] = butter(2, cutoff_frequency/(Fs/2), 'high');
% Apply the highpass filter to remove DC offset
dc_offset_removed_signal_hp = filter(b_hp, a_hp, magnitude_demodulated_signal);
figure(1);
pspectrum(dc_offset_removed_signal_hp,Fs/4);
title('Final Demodulated Signal in Frequency Domain');

figure(2);
plot(t,dc_offset_removed_signal_hp);
title('Final Demodulated Signal in Time Domain');
xlabel('Time (s)');
ylabel('Amplitude');

sound(dc_offset_removed_signal_hp,Fs/4);

%{
dc_offset_removed_signal = HP_IIR;

sound(dc_offset_removed_signal,Fs/4);

figure(1); 
pspectrum(dc_offset_removed_signal,Fs/4);
title('Real-Part Frequency Response of two-stage FIR LP');


figure(2);
plot(t,dc_offset_removed_signal);
title('Real-Part After Mixing in Time Domain (Two-stage FIR)');
legend('Mixed Signal','FIR Filtered');
xlabel('Time (s)');
ylabel('Amplitude');




figure(2);
subplot(2,1,1);
plot(t,mixed_signal,'b');
hold on;
plot(t,y_single_stage,'r');
legend('Original','After LP Filtered');
title(' Real-Part FIR LP result in Time Domain');
xlabel('Time (s)');
ylabel('Amplitude');

subplot(2,1,2);
plot(t,mixed_signal2,'b');
hold on;
plot(t,y_single_stage2,'r');
legend('Original','After LP Filtered');
title('Imaginary-Part FIR LP result in Time Domain');
xlabel('Time (s)');
ylabel('Amplitude');

figure(1);
subplot(2,1,1);
pspectrum(mixed_signal,fs);
title('Real-Part After Mixing in Frequency Domain');
subplot(2,1,2);
pspectrum(mixed_signal2,fs);
title('Imaginary-Part After Mixing in Frequency Domain');

figure(2);
subplot(2,1,1);
plot(t,mixed_signal);
title('Real-Part After Mixing in Time Domain');
xlabel('Time (s)');
ylabel('Amplitude');
subplot(2,1,2);
plot(t,mixed_signal2);
title('Imaginary-Part After Mixing in Time Domain');
xlabel('Time (s)');
ylabel('Amplitude');



Fc = (4000+500)/Fs; 

m = 20; 
N1 = 2*m+1;

% Single Stage FIR
h_single_stage = zeros(1, 2*m + 1);
for n = 1:m
    h_single_stage(n) = 2 * Fc * sin(n * 2 * pi * Fc ) / (n * 2 * pi * Fc);
end

h_single_stage = [fliplr(h_single_stage(1:m)) 2 * Fc h_single_stage(1:m)];

w = blackman(N1)';
h_single_stage = h_single_stage.*w;


h_two_stage = conv(h_single_stage, h_single_stage);
decimationFactor = 2;

signal_FIR1 = conv(h_single_stage,mixed_signal);

signal_FIR_2 = conv(h_single_stage, mixed_signal2);

FIR_downsampled1 = decimate(signal_FIR1, decimationFactor);

FIR_downsampled2 = decimate(signal_FIR_2, decimationFactor);

magnitude_demodulated_signal = sqrt(FIR_downsampled1.^2 + FIR_downsampled2.^2);

% IIR High-pass around 10 Hz 

cutoff_frequency = 10; % Adjust as needed
[b, a] = butter(2, cutoff_frequency/(Fs/2), 'high');

x = magnitude_demodulated_signal;     % 生成白噪声信号（为了与默认512点FFT的pspectrum归一化）


Order = 2;
delay_x = zeros(Order+1,1);      % 为输入（x）创建延迟线并初始化为零
delay_y = zeros(Order,1);        % 为输出（y）创建延迟线并初始化为零

for i=1:length(x)
    for n=Order+1:-1:2          
        delay_x(n) = delay_x(n-1);      % 将输入延迟线向右移动1个样本（最后一个样本被忘记）
    end
    delay_x(1) = x(i);              % 将最新的输入样本插入到延迟线的开头

    Accumulator = 0;                    % 清零累加器
    for n=1:Order+1                     % 应用前向系数（FIR）                     
        Accumulator = Accumulator + b(n)*delay_x(n);    % b(n)*x(i-n)
    end
    
    for n=2:Order+1                       % 应用反馈系数                     
        Accumulator = Accumulator - a(n)*delay_y(n-1);    % -a(n)*y(i-n-1)
    end
    pulse_IIR3(i) = Accumulator;        % 输出值
    for n=Order:-1:2          
        delay_y(n) = delay_y(n-1);      % 将输出延迟线向右移动1个样本（最后一个样本被忘记）
    end
    delay_y(1) = Accumulator;           % 将最新的输出存储到延迟线中
end

% Apply the highpass filter to remove DC offset
%dc_offset_removed_signal_hp = filter(b_hp, a_hp, magnitude_demodulated_signal);

dc_offset_removed_signal_hp = pulse_IIR3;

sound(dc_offset_removed_signal_hp, Fs/2);

figure;
pspectrum(mixed_signal,Fs) % spectrum of output signal (frequency response)

figure;
pspectrum(FIR_downsampled1,Fs/2) % spectrum of output signal (frequency response)

figure;
pspectrum(dc_offset_removed_signal_hp,Fs/2)

figure;
t1 = linspace(0, duration, 262184);
plot(t1(1:131092),dc_offset_removed_signal_hp,'b');
hold on;
plot(t,y1,'r');
%}