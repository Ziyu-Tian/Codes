k_t=62.4E-3;
B=8.4445E-6;
T_sf=6.3E-3;
T_e_2=data_2(:,3)*k_t;
T_e_4=data_4(:,3)*k_t;
T_m_2=T_e_2-B.*data_2(:,4)-T_sf;
T_m_4=T_e_4-B.*data_4(:,4)-T_sf;
plot(Resistance_2,T_e_2,'r-');
hold on;
plot(Resistance_2,T_m_2,'b-');
hold on;
plot(Resistance_4,T_e_4,'g-');
hold on;
plot(Resistance_4,T_m_4,'y-');
xlabel('Reisitance,ohm');
ylabel('Torque,Nm');
legend('T_{e} Sim','T_{mech} Sim','T_{e} Exp','T_{mech} Exp');
title('Torque vs Resistance')