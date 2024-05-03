% Demonstration of auto/crosscorrelation and matched filtering
% in MATLAB
% J Neasham, Newcastle University, 2020

%%
clc;                %clear command window
clear all;          %clear workspace variables
close all;          %close previous plots

N = 1000;            %no of samples in signal

x=randn(1,N);       % generate normally distributed, random sequence with variance of 1
y=randn(1,N);       % generate another normally distributed, random sequence with variance of 1

fprintf('Mean of x = %f Variance of x = %f\n', mean(x), var(x)); 

plot(x)
hold;
plot(y,'r')
xlabel('n');
ylabel('Amplitude');
legend('x','y');
%{
%%
n = -(N-1):N-1;     %lag values
Cxx = xcorr(x,x);   %autocorrelation  
figure;
plot(n,Cxx)
hold;
Cxy = xcorr(x,y);   %crosscorrelation
plot(n,Cxy,'r')
xlabel('lag (samples)');
ylabel('Correlation');
legend('Cxx','Cxy');
title('Auto and cross correlation of random signals');


%%
y = [x x x x x];        %repeat x 5 times
Cyy = xcorr(y,y);       %autocorrelation
figure;
plot(Cyy)
xlabel('lag (samples)');
ylabel('Correlation');
legend('Cyy');
title('Autocorrelation of periodic signal')
%}

%%
y = [zeros(1,2*N) x zeros(1,2*N)];      %insert silence either side of signal x
y = y + 5*randn(1,5*N);               %add background noise

figure; plot(y);
Cxy = conv(fliplr(x),y);                %cross correlate (matched filter is convolution with time reversed signal)
figure;
plot(Cxy)
xlabel('lag (samples)');
ylabel('Correlation');
legend('Cxy');
title('Cross-correlation of signal + noise')
pause(1000);