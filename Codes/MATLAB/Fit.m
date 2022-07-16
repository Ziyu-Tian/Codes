
p=polyfit(angular_speed,T_cal,1);
Fitted=p(1)*angular_speed+p(2);
plot(angular_speed,T_cal,'b-');
hold on;
plot(angular_speed,Fitted,'r-');
xlabel('Speed');
ylabel('Torque');
legend('T_{cal}','T_{cal} Fitted');
title('T_{cal} and Fitted Line')