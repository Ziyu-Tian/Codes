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

frequencies = linspace(0, Fs, N);
frequencies = frequencies / 1000; % Change unit to kHz

m = 2;

[b1, a1] = butter(m, [fp1, fp2]/(fs/2), 'bandpass');

b = [0.0000273 0 -0.00005461 0 0.0000273];  % numerator coefficients for IIR filter
a = [1.0077 -0.77 1.455 -0.598 0.5776];  % denominator coefficients for IIR filter


xf_IIR_function = filter(b1,a1,y1);


% Mix Signal
fc = 20000;
lo_frequency = fc;  % Local oscillitor 
lo_signal1 = cos(2 * pi * lo_frequency * t)';
lo_signal2 = sin(2 * pi * lo_frequency * t)';
mixed_signal = xf_IIR_function .* lo_signal1;
mixed_signal2 = xf_IIR_function.* lo_signal2;

Mixed1 = fft(mixed_signal,N);
Amplitude1 = 20*log10(abs(Mixed1)/N);

Mixed2 = fft(mixed_signal2,N);
Amplitude2 = 20*log10(abs(Mixed2)/N);

figure(1);
subplot(2, 1, 1);
plot(frequencies(1:N/2), Amplitude1(1:N/2),'b');
title('Signal-1 after mixing');
xlabel('Frequency (kHz)');
ylabel('Amplitude (dB)');

subplot(2, 1, 2);
plot(frequencies(1:N/2), Amplitude2(1:N/2),'b');
title('Signal-2 after mixing');
xlabel('Frequency (kHz)');
ylabel('Amplitude (dB)');


figure(2);
plot(t, mixed_signal,'b');
title('Time Domain');
xlabel('Time (s)');
ylabel('Amplitude');