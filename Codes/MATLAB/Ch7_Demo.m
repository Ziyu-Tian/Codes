% Demonstration of LMS adaptive filtering
% in MATLAB
% J Neasham, Newcastle University, 2020

clc;
close all;
clear all;

fs = 8000;              %sampling frequency
Ts = 1/fs;              %sample period
Tmax = 2;               %signal duration
t = 0:Ts:Tmax;          %generate discrete time values (nTs)
f = 10;                 %signal frequency
N = 3;                  %number of filter coefficients
mu = 0.001;              %adaptive step size

%adaptive interference cancellation (direct system model)

Sig = sin(2*pi*f*t);                %generate sinusoid (double precision floating point)
Channel = [2.5 5 1];                %interference channel coefficients (the unknown coupling function)
Reference = randn(1,length(Sig));   %reference signal (interference source)
Interference = conv(Channel,Reference);     %interference coupled to signal
Noise = randn(1,length(Sig));        %generate white noise signal (unknown additive noise)

y = 1.0*Sig + Interference(1:length(Sig)) + 0.1*Noise;  % signal + interference + random noise

x = zeros(N,1);      %create delay line for input (x) and initialise to zero
w = zeros(N,1);            %adaptive filter coefficients (initialised to zero)

for i=1:length(y)
    for n=N:-1:2          
        x(n) = x(n-1);      %shift input delay line to the right by 1 sample (last sample is forgotten)
    end
    x(1) = Reference(i);        %insert most recent reference sample at start of delay line
    I_est = 0;                  %zero accumulator
    for n=1:N                   %calculator filter output                     
        I_est = I_est + w(n)*x(n);    %b(n)*x(i-n)
    end
    e(i) = y(i)-I_est;          %calculate error (& estimate of Sig)
    for n=1:N 
        w(n) = w(n) + 2*mu*e(i)*x(n);   %apply LMS update
    end
end

figure;
subplot(2,1,1); plot(t,y);
title('signal + interference');
xlabel('time(s)');
subplot(2,1,2); plot(e)
title('Estimated signal (error)');
xlabel('no of iterations');
figure;
plot(w)
hold;
plot(Channel,'r');
title('Adaptive filter coefficients');
legend('Filter', 'Channel');
xlabel('n');

pause(1000);

