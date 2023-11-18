clear;clc;close all;


% ��ȡ��Ƶ�ļ�
filename = '/Users/ziyutian/Downloads/Year_3/AM files 2023/Tian Gao.wav';
[y, Fs] = audioread(filename);

% ������Ƶʱ��
duration = length(y) / Fs;

%{
% ʱ��ͼ��
time = linspace(0, duration, length(y));
figure;
subplot(2, 1, 1);
plot(time, y);
title('Time Domain');
xlabel('Time (s)');
ylabel('Amplitude');

%}



% Ƶ��ͼ��
N = length(y);
frequencies = linspace(0, Fs, N);
Y = fft(y);
amplitude = abs(Y)/N;
subplot(2, 1, 2);
plot(frequencies(1:N/2), amplitude(1:N/2));
title('Frequency Domain');
xlabel('Frequency(Hz)');
ylabel('Amplitude');


% Ϊͼ����ӱ�ǩ�ͱ���
