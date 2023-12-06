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


[b1, a1] = butter(2, [fp1, fp2]/(fs/2), 'bandpass');
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

%-----------------------------------------Single-stage Version---------------------------------------
%{
Fc = 4000/(Fs); 
m = 264; 
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
toc;
% Load calculation = 264 x 96000 = 25.3 M
% Calculation Time = 0.2342s
%-----------------------------------------Single-stage Version End---------------------------------------
%}

%-------------------------------------------Stage-two Version-----------------------------------

% Single Stage FIR
Fc = 4000/(Fs*0.5); % transition width = 4 to 9 (5kHz), decimation factor = 2
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
m2 = 66; % transition width = 4 to 5 (1kHz), decimation factor = 4
N2 = 2*m2+1;
Fc = 4000/(0.25*Fs);

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
% Calculation Time = 0.789732s
%-------------------------------------------Stage-two Version End-----------------------------------



%y_downsampled = y_single_stage(1:newDecimationFactor:end);

figure;
pspectrum(mixed_signal,Fs) % spectrum of output signal (frequency response)

figure;
pspectrum(y_two_stage,Fs/4) % spectrum of output signal (frequency response)


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