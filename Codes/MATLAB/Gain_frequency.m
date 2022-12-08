data_1 = xlsread('gainFrequency.xlsx',1,'A2:D8');
data_2 = xlsread('gainFrequency.xlsx',2,'A2:D9');

x_1 = data_1(:,1);
x_1 = 2*log10(x_1);

x_2 = data_1(:,2);

y_1 = data_1(:,4);
y_1 = 20*log10(y_1);

y_2 = data_2(:,4);
y_2 = 20*log10(y_2);


plot(x_1,y_1,'b-');

xlabel('Frequency, dB(kHz)');
ylabel('Gain, dB');
title('Gain versus Frequency');

pause(500);