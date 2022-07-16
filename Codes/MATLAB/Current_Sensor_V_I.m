data=xlsread('Test_3.xlsx',1,'A2:G22');
x_1=data(:,1);
x_2=data(:,3);
y_1=data(:,5);
y_2=data(:,7);
plot(x_1,y_1,'b-');
hold on;
plot(x_2,y_2,'r-');
xlabel('current, A');
ylabel('voltage, V');
legend('Exp V','Cal V');
title('Current Sensor Voltage verses Current ')