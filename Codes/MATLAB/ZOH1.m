%{
Tcont = 0.001;
tc = 0:Tcont:2; % Time vector 

A = sin(2*pi*1*tc); %Genrate Signal 

Tsample = 0.1;
ts = 0:Tsample:2; % Sample Time vector

As = sin(2*pi*1*ts);

B= sin(2*pi*1*(tc-Tsample/2));
figure(3);
hold on
plot(tc,A,'b');
stairs(ts,As,'c');
plot(tc,B,'r');
hold off;

Tsample = 0.51;
ts = 0:Tsample:2; % Sample Time vector
As = sin(2*pi*1*ts);

B= sin(2*pi*1*(tc-Tsample/2));
figure(4);
hold on
plot(tc,A,'b');
stairs(ts,As,'c');
plot(tc,B,'r');
hold off;
%}
% This program quantizes a signal to n bits.
 % It demonstrates, in simple terms, the operation of a typical ADC. % This code assumes the signal is normalized; between -1 and +1.
close all
n=3;
Tsample = 0.001
ts = 0:Tsample:2;
x = sin(2*pi*1*ts)

%x=sawtooth(2*pi*1*tc); %...you might wish to try a sawtooth waveform 14
xn1=x+1
xn2 = xn1*2^(n-1);
xn3 = floor(xn2);
18 xn4=xn3/(2^(n-1));
19 xn5=xn4-1;
20
21 figure(1)
22 hold on;
23 stem(x,'b');
24 hold on;
25 stem(xn5,'r');
26 legend('exact','quantized')
27 title(sprintf('Sampled signal versus Quantized signal'));
28 hold off
29
30 xerror = x - xn5 31
32 figure(2)
33
34 stem(xerror,'g');
35 title(sprintf('Quantization Error'));
36 ylim([0 1])