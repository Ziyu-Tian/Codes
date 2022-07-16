table_1=xlsread('Table_1.xlsx',1,'A2:J13');
angular_speed=table_1(:,7);
T_cal=table_1(:,9);
T_exp=table_1(:,10);
plot(angular_speed,T_cal,'r-');
hold on;
plot(angular_speed,T_exp,'b-');
hold on;
plot(angular_speed,Fitted,'g-')
xlabel('speed, rad/s');
ylabel('torque, Nm');
title('No Load Mechanical Torque vs Speed');
legend('T_{e} cal','T_{e} exp','T_{e} cal Fitted','T_{final}')


