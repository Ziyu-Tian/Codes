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

[b,a] = butter(2,[fp1,fp2]/(fs/2),'bandpass');   % ���Butterworth IIR�˲��� (MATLAB����)

x = y1;     % ���ɰ������źţ�Ϊ����Ĭ��512��FFT��pspectrum��һ����



delay_x = zeros(Order+1,1);      % Ϊ���루x�������ӳ��߲���ʼ��Ϊ��
delay_y = zeros(Order,1);        % Ϊ�����y�������ӳ��߲���ʼ��Ϊ��

for i=1:length(x)
    for n=Order+1:-1:2          
        delay_x(n) = delay_x(n-1);      % �������ӳ��������ƶ�1�����������һ�����������ǣ�
    end
    delay_x(1) = x(i);              % �����µ������������뵽�ӳ��ߵĿ�ͷ

    Accumulator = 0;                    % �����ۼ���
    for n=1:Order+1                     % Ӧ��ǰ��ϵ����FIR��                     
        Accumulator = Accumulator + b(n)*delay_x(n);    % b(n)*x(i-n)
    end
    
    for n=2:Order+1                       % Ӧ�÷���ϵ��                     
        Accumulator = Accumulator - a(n)*delay_y(n-1);    % -a(n)*y(i-n-1)
    end
    pulse_IIR2(i) = Accumulator;        % ���ֵ
    for n=Order:-1:2          
        delay_y(n) = delay_y(n-1);      % ������ӳ��������ƶ�1�����������һ�����������ǣ�
    end
    delay_y(1) = Accumulator;           % �����µ�����洢���ӳ�����
end

figure(2);
pspectrum(pulse_IIR2(1:1780),fs)            % ����źŵ�Ƶ�ף�Ƶ����Ӧ��
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