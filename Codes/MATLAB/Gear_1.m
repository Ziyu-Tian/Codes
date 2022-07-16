Data=xlsread('Gear_2.xlsx',1,'A2:L5');
x=Data(:,4);
y_1=Data(:,6);
y_2=Data(:,10);
plot(x,y_1,'r-');
hold on;
plot(x,y_2,'b-');
xlabel("Output Torque (Nm)");
ylabel("Inout Torque (Nm)");
legend('Theoretical Torque','Actrual Torque');
title('Inoput Torque - Output Torque')