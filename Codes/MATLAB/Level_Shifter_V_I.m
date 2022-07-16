data=xlsread('Test_4.xlsx',1,'A2:E22');
x=data(:,1);
y_1=data(:,2);
y_2=data(:,4);
y_3=data(:,5);
plot(x,y_1,'k-');
hold on;
plot(x,y_2,'r-');
hold on;
plot(x,y_3,'b-');
xlabel('current, A');
ylabel('voltage, V');
legend('V R_cs','V_out Cal','V_out Exp');
title('Level Shifter Voltage verses Current ');

