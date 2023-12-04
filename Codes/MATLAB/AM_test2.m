% ����AM�ź�
filename = 'Ziyu Tian.wav';
[y, fs] = audioread(filename);
fc = 20000;
t = (0:length(y)-1)/fs;
% ��������
lo_frequency = fc;  % ��������Ƶ�ʣ����Ե���
lo_signal1 = cos(2 * pi * lo_frequency * t)';
lo_signal2 = sin(2 * pi * lo_frequency * t)';

fc_low = 10600;  
fc_high = 20400;
m = 2;          
[b, a] = butter(m, [fc_low, fc_high] / (fs / 2), 'bandpass');
y_filtered = filter(b, a, y);

% ��Ƶ
mixed_signal = y_filtered .* lo_signal1;

mixed_signal2 = y_filtered .* lo_signal2;


% �˲��Ͳ�������ȡԭʼ��Ϣ�ź�
filtered_signal = lowpass(mixed_signal, 4000, fs); 
filtered_signal2 = lowpass(mixed_signal2, 4000, fs);

demodulated_signal = abs(filtered_signal);  % ȡ���

% ʹ�� pspectrum ����Ƶ��ͼ
figure;

% AM�źŵ�Ƶ��
subplot(3, 1, 1);
pspectrum(y, fs);
title('AM�źŵ�Ƶ��');

% ��Ƶ����źŵ�Ƶ��
subplot(3, 1, 2);
pspectrum(mixed_signal, fs);
title('mix����źŵ�Ƶ��');

% �������źŵ�Ƶ��
subplot(3, 1, 3);
pspectrum(filtered_signal, fs);
title('low pass filter����źŵ�Ƶ��');

magnitude_demodulated_signal = sqrt(filtered_signal.^2 + filtered_signal2.^2);

dc_offset_removed_signal = magnitude_demodulated_signal - mean(magnitude_demodulated_signal);

cutoff_frequency = 20; % Adjust as needed
[b_hp, a_hp] = butter(2, cutoff_frequency/(Fs/2), 'high');

% Apply the highpass filter to remove DC offset
dc_offset_removed_signal_hp = filter(b_hp, a_hp, magnitude_demodulated_signal);

% Play back the signal
sound(filtered_signal, Fs);

figure;

pspectrum(dc_offset_removed_signal_hp,Fs)