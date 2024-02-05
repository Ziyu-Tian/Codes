filename = 'Ziyu Tian.wav';
[y, fs] = audioread(filename);

t = (0:length(y)-1)/fs;


fc_low = 10600;  
fc_high = 20400;
m = 2;          
[b, a] = butter(m, [fc_low, fc_high] / (fs / 2), 'bandpass');
y_filtered = filter(b, a, y);

% Mix Signal
fc = 20000;
lo_frequency = fc;  % Local oscillitor 
lo_signal1 = cos(2 * pi * lo_frequency * t)';
lo_signal2 = sin(2 * pi * lo_frequency * t)';
mixed_signal = y_filtered .* lo_signal1;
mixed_signal2 = y_filtered .* lo_signal2;

% Low-pass FIR
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
% 
%h_two_stage = conv(h_single_stage, h_single_stage);
decimationFactor = 1;
signal_FIR1 = conv(h_single_stage,mixed_signal);
FIR_downsampled1 = decimate(signal_FIR1, decimationFactor);
signal_FIR2 = conv(h_single_stage,mixed_signal2);
FIR_downsampled2 = decimate(signal_FIR2, decimationFactor);



%{
filtered_signal = lowpass(mixed_signal, 4000, fs); 
filtered_signal2 = lowpass(mixed_signal2, 4000, fs);
%}

demodulated_signal = abs(FIR_downsampled1);  

figure;


subplot(3, 1, 1);
pspectrum(y, fs);
title('AM Signal');


subplot(3, 1, 2);
pspectrum(mixed_signal, fs);
title('After Mixing');


subplot(3, 1, 3);
pspectrum(FIR_downsampled1, fs);
title('After low pass filter');

magnitude_demodulated_signal = sqrt(FIR_downsampled1.^2 + FIR_downsampled2.^2);

dc_offset_removed_signal = magnitude_demodulated_signal - mean(magnitude_demodulated_signal);

cutoff_frequency = 20; % Adjust as needed
[b_hp, a_hp] = butter(2, cutoff_frequency/(Fs/2), 'high');

% Apply the highpass filter to remove DC offset
dc_offset_removed_signal_hp = filter(b_hp, a_hp, magnitude_demodulated_signal);

% Play back the signal
sound(dc_offset_removed_signal_hp, Fs);

figure;

pspectrum(dc_offset_removed_signal_hp,Fs)