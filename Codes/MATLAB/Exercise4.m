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
t = linspace(0, duration, N);

% Hamming Windows applied 
window = hamming(N);
y1 = [y; zeros(N - n, 1)];
y1 = y1 .* window;

Order = 4;

[b,a] = butter(2,[fp1,fp2]/(fs/2),'bandpass');   % 设计Butterworth IIR滤波器 (MATLAB函数)

x = y1;     % 生成白噪声信号（为了与默认512点FFT的pspectrum归一化）



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
    pulse_IIR2(i) = Accumulator;        % 输出值
    for n=Order:-1:2          
        delay_y(n) = delay_y(n-1);      % 将输出延迟线向右移动1个样本（最后一个样本被忘记）
    end
    delay_y(1) = Accumulator;           % 将最新的输出存储到延迟线中
end

figure(2);
pspectrum(pulse_IIR2(1:1780),fs)            % 输出信号的频谱（频率响应）
legend('IIR');
title('Frequency response');



lo_frequency = 20000;  % Local oscillitor 
lo_signal1 = cos(2 * pi * lo_frequency * t)';
lo_signal2 = sin(2 * pi * lo_frequency * t)';
mixed_signal = pulse_IIR2 .* lo_signal1';
mixed_signal2 = pulse_IIR2.* lo_signal2';

frequencies = linspace(0, Fs, N);
frequencies = frequencies / 1000; % Change unit to kHz

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

cutoff_frequency = 10; % Adjust as needed
[b_hp, a_hp] = butter(2, cutoff_frequency/(Fs/2), 'high');

% Apply the highpass filter to remove DC offset
dc_offset_removed_signal_hp = filter(b_hp, a_hp, magnitude_demodulated_signal);

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