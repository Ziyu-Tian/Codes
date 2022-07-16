x=data(:,1);
y=data(:,3);
k_t=62.4e-3;
T_e=y*k_t;
speed=data(:,4);
T_m=T_e-(14.32e-6)*speed-7e-3;
plot(x,T_e,'b-');
hold on;
plot(x,T_m,'r-');
xlabel('load resistance,ohm');
ylabel('torque,Nm');
legend('Elec Torque','Mech Torque');
tiele('Electromagnetic / Mechanical Torques vs Load Resistance')
