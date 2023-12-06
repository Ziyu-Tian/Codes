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


m = 2;
[b1, a1] = butter(m, [fp1, fp2]/(fs/2), 'bandpass');
b = [0.0000273 0 -0.00005461 0 0.0000273];  % numerator coefficients for IIR filter
a = [1.0077 -0.77 1.455 -0.598 0.5776];  % denominator coefficients for IIR filter


xf_IIR_function = filter(b1,a1,y1);
lo_frequency = 20000;  % Local oscillitor 
lo_signal1 = cos(2 * pi * lo_frequency * t)';
lo_signal2 = sin(2 * pi * lo_frequency * t)';
mixed_signal = xf_IIR_function .* lo_signal1;
mixed_signal2 = xf_IIR_function.* lo_signal2;

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
 

%h_two_stage = conv(h_single_stage, h_single_stage);
decimationFactor = 1;

%{
tic;
signal_FIR1 = conv(h_single_stage,mixed_signal);
toc;

tic;
signal_FIR_2 = conv(h_two_stage, mixed_signal);
toc;
%}

x =mixed_signal;

y_single_stage = zeros(size(x));
for i = 1:length(x)
    for j = 1:length(h_single_stage)
        if i - j + 1 > 0
            y_single_stage(i) = y_single_stage(i) + x(i - j + 1) * h_single_stage(j);
        end
    end
end

figure;
pspectrum(mixed_signal,Fs) % spectrum of output signal (frequency response)

figure;
pspectrum(y_single_stage,Fs) % spectrum of output signal (frequency response)



%{
magnitude_demodulated_signal = sqrt(FIR_downsampled1.^2 + FIR_downsampled2.^2);

cutoff_frequency = 10; % Adjust as needed
[b_hp, a_hp] = butter(2, cutoff_frequency/(Fs/2), 'high');

% Apply the highpass filter to remove DC offset
dc_offset_removed_signal_hp = filter(b_hp, a_hp, magnitude_demodulated_signal);

sound(dc_offset_removed_signal_hp, Fs);

figure;
pspectrum(mixed_signal,Fs) % spectrum of output signal (frequency response)

figure;
pspectrum(FIR_downsampled1,Fs) % spectrum of output signal (frequency response)

figure;
pspectrum(dc_offset_removed_signal_hp,Fs)

figure;
t1 = linspace(0, duration, 262184);
plot(t1,dc_offset_removed_signal_hp,'b');
hold on;
plot(t,y1,'r');

%}