clear;clc;close all;


% 读取音频文件
filename = '/Users/ziyutian/Downloads/Year_3/AM files 2023/Tian Gao.wav';
[y, Fs] = audioread(filename);

% 计算音频时长
duration = length(y) / Fs;

%{
% 时域图像
time = linspace(0, duration, length(y));
figure;
subplot(2, 1, 1);
plot(time, y);
title('Time Domain');
xlabel('Time (s)');
ylabel('Amplitude');

%}



% 频域图像
N = length(y);
frequencies = linspace(0, Fs, N);
Y = fft(y);
amplitude = abs(Y)/N;
subplot(2, 1, 2);
plot(frequencies(1:N/2), amplitude(1:N/2));
title('Frequency Domain');
xlabel('Frequency(Hz)');
ylabel('Amplitude');


% 为图像添加标签和标题
