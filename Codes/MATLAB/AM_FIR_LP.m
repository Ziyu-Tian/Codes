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


xf_IIR_function = filter(b1,a1,y1);
lo_frequency = 20000;  % Local oscillitor 
lo_signal1 = cos(2 * pi * lo_frequency * t)';
lo_signal2 = sin(2 * pi * lo_frequency * t)';
mixed_signal = xf_IIR_function .* lo_signal1;
mixed_signal2 = xf_IIR_function.* lo_signal2;

frequencies = linspace(0, Fs, N);
frequencies = frequencies / 1000; % Change unit to kHz
%{
%-----------------------------------------Single-stage Version---------------------------------------

Fc = 4000/(Fs); % Normalized cutting frequency 
m = 264; 
N1 = 2*m+1; % N1 = 5.5 / (1000/96000) = 528 (529)

tic;
h_single_stage = zeros(1, 2*m + 1); % Intialize the FIR array with zeros
for n = 1:m
    h_single_stage(n) = 2 * Fc * sin(n * 2 * pi * Fc ) / (n * 2 * pi * Fc); 
    % Truncate the Low-pass Impluse Response Fucntion with m coefficients (first half of the FIR array)
end

h_single_stage = [fliplr(h_single_stage(1:m)) 2*Fc h_single_stage(1:m)];
% Construct the fliped another half, the 2Fc component in the middle and the original half of FIR Coefficients Array
toc;

w = blackman(N1)'; % Applying the suitable window function 
h_single_stage = h_single_stage.*w;

% Single stage convolution 
x =mixed_signal;
%x = randn(1, 262144) * sqrt(512); % Generate a Random Noises with same sampling points of AM signal (N = 262144)

y_single_stage = zeros(size(x));
for i = 1:length(x)
    for j = 1:length(h_single_stage)
        if i - j + 1 > 0
            y_single_stage(i) = y_single_stage(i) + x(i - j + 1) * h_single_stage(j); % Convolution of h_FIR() and x()
        end
    end
end

% Single stage convolution 
x =mixed_signal2;
%x = randn(1, 262144) * sqrt(512); % Generate a Random Noises with same sampling points of AM signal (N = 262144)

y_single_stage2 = zeros(size(x));
for i = 1:length(x)
    for j = 1:length(h_single_stage)
        if i - j + 1 > 0
            y_single_stage2(i) = y_single_stage2(i) + x(i - j + 1) * h_single_stage(j); % Convolution of h_FIR() and x()
        end
    end
end

figure(1); 
pspectrum(x,Fs);
hold on;
pspectrum(y_single_stage,Fs);
title('Testing Frequency Response of FIR LP');
legend('Before Filtered','After Filtered');
% spectrum of output signal (frequency response)
%{
title('Real-Part After Mixing in Frequency Domain');
subplot(2,1,2);
pspectrum(mixed_signal2,fs);
title('Imaginary-Part After Mixing in Frequency Domain');
%}

% Load calculation = 264 x 96000 = 25.3 M
% Calculation Time = 0.2342s
%-----------------------------------------Single-stage Version End---------------------------------------
%}



%-------------------------------------------Stage-two Version-----------------------------------

% Single Stage FIR
Fc = 4000/(0.5*Fs); % transition width = 4 to 9 (5kHz), decimation factor = 2
m = 53; 
N1 = 2*m+1;

h_single_stage = zeros(1, 2*m + 1);
for n = 1:m
    h_single_stage(n) = 2 * Fc * sin(n * 2 * pi * Fc ) / (n * 2 * pi * Fc);
end

h_single_stage = [fliplr(h_single_stage(1:m)) 2 * Fc h_single_stage(1:m)];

w = blackman(N1)';
h_single_stage = h_single_stage.*w;

% Single stage convolution 
x =mixed_signal;
x1 = mixed_signal2;


y_single_stage = zeros(size(x));
for i = 1:length(x)
    for j = 1:length(h_single_stage)
        if i - j + 1 > 0
            y_single_stage(i) = y_single_stage(i) + x(i - j + 1) * h_single_stage(j);
        end
    end
end

y_single_stage2 = zeros(size(x1));
for i = 1:length(x1)
    for j = 1:length(h_single_stage)
        if i - j + 1 > 0
            y_single_stage2(i) = y_single_stage2(i) + x(i - j + 1) * h_single_stage(j);
        end
    end
end

% Second Stage FIR
m2 = 132; % transition width = 4 to 5 (1kHz), decimation factor = 4
N2 = 2*m2+1;
Fc = 4000/(0.25*Fs);

h_two_stage = zeros(1, 2*m2 + 1);
for n = 1:m2
    h_two_stage(n) = 2 * Fc * sin(n * 2 * pi * Fc ) / (n * 2 * pi * Fc);
end

h_two_stage2 = zeros(1, 2*m2 + 1);
for n = 1:m2
    h_two_stage2(n) = 2 * Fc * sin(n * 2 * pi * Fc ) / (n * 2 * pi * Fc);
end

h_two_stage = [fliplr(h_two_stage(1:m2)) 2 * Fc h_two_stage(1:m2)];
h_two_stage2 = [fliplr(h_two_stage2(1:m2)) 2 * Fc h_two_stage2(1:m2)];

w = blackman(N2)';
h_two_stage = h_two_stage.*w;
h_two_stage2 = h_two_stage2.*w;

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

y_two_stage2 = zeros(size(x1));
for i = 1:length(x1)
    for j = 1:length(h_two_stage2)
        if i - j + 1 > 0
            y_two_stage2(i) = y_two_stage2(i) + x1(i - j + 1) * h_two_stage2(j);
        end
    end
end
% Load calculation = 53 x 96000 x 0.5 + 133 x 96000 x 0.25 = 5.73 M
% Calculation Time = 0.789732s
%-------------------------------------------Stage-two Version End-----------------------------------




%{
figure;
pspectrum(mixed_signal,Fs) % spectrum of output signal (frequency response)
%}



magnitude_demodulated_signal = sqrt(y_two_stage.^2 + y_two_stage2.^2);

Order = 4;
delay_x = zeros(Order+1,1);      % Create Delay Array for x and intialize with zeros (input)
delay_y = zeros(Order,1);        % Create Delay Array for x and intialize with zeros (output)

for i=1:length(x)
    for n=Order+1:-1:2          
        delay_x(n) = delay_x(n-1);      % Right-shift the delay_x Array 
    end
    delay_x(1) = x(i);              % Input each sample from signal x to the begining of delay_x

    Accumulator = 0;                    % Clear the Accumulator until the end of x-signal
    for n=1:Order+1                     % Calculate the result from input x                     
        Accumulator = Accumulator + b(n)*delay_x(n);    % + b(n)*x(i-n)
    end
    
    for n=2:Order+1                       % Calculate the result from output y                    
        Accumulator = Accumulator - a(n)*delay_y(n-1);    % -a(n)*y(i-n-1)
    end
    HP_IIR(i) = Accumulator;        % Store the output data of this sample x(i)
    for n=Order:-1:2          
        delay_y(n) = delay_y(n-1);      % Right-shift the delay_y Array 
    end
    delay_y(1) = Accumulator;
end


cutoff_frequency = 1; % Adjust as needed
[b_hp, a_hp] = butter(2, cutoff_frequency/(Fs/2), 'high');



% Apply the highpass filter to remove DC offset
dc_offset_removed_signal_hp = filter(b_hp, a_hp, magnitude_demodulated_signal);

sound(dc_offset_removed_signal_hp, Fs);

figure(1);
subplot(2,1,1);
pspectrum(dc_offset_removed_signal_hp,Fs/4);
title('Final Demodulated Signal in Frequency Domain');

subplot(2,1,2);
plot(t,dc_offset_removed_signal_hp);
title('Final Demodulated Signal in Time Domain');
xlabel('Time (s)');
ylabel('Amplitude');

sound(dc_offset_removed_signal_hp(1:2620),Fs/4);
%{
figure;
t1 = linspace(0, duration, 262184);
plot(t1,dc_offset_removed_signal_hp,'b');
hold on;
plot(t,y1,'r');
%}
