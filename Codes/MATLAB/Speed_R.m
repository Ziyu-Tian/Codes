data_3=xlsread('Test_2.xlsx',1,'A2:F34');
data_4=xlsread('Test_2.xlsx',1,'A2:F31');
Resistance_1=data_1(:,1);
Resistance_3=data_3(:,1);
speed_1=data_1(:,4);
speed_3=data_3(:,4);
plot(Resistance_1,speed_1,'b-');
hold on;
plot(Resistance_3,speed_3,'r-');
xlabel('load resistance, Ohm');
ylabel('speed, rad/s');
title('Rotational Speed vs Load Resistance ');
legend('Sim Speed','Exp Speed')