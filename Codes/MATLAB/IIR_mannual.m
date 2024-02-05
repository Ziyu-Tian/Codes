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

m = 2;

[b, a] = butter(m, [fp1, fp2]/(fs/2), 'bandpass');

% 生成输入信号
x = y1;


% 初始化输出信号
y = zeros(size(x));

% 应用差分方程
for n = 5:length(x)
    y(n) = b(1)*x(n) + b(2)*x(n-1) + b(3)*x(n-2) + b(4)*x(n-3) + b(5)*x(n-4) - a(1)*y(n-1) - a(2)*y(n-2) - a(3)*y(n-3) - a(4)*y(n-4);
end

y = y'/Fs;

y = fft(y);
y = abs(y)/N;

% 绘制功率谱密度
figure;
%pspectrum(x, Fs);

plot(frequencies(1:N/2),y(1:N/2));
%pspectrum(y(1:1093),Fs);

