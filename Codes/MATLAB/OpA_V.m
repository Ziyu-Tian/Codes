data_3=xlsread('Test_7.xlsx',1,'A2:C34');
y_1=data_3(:,2);
y_2=data_3(:,3);
x=data_3(:,1);
plot(x,y_1,'b-');
hold on;
plot(x,y_2,'r-');
xlabel('Generator Voltage, V');
ylabel('op amp output, V');
legend('Vout Sim','Vout Exp');
title('Op amp Voltage vs Generator Voltage ')