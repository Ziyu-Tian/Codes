Vout_sim_plus=data_1(:,3);
Vout_sim_minus=data_1(:,4);
Vout_exp_plus=data_2(:,3);
Vout_exp_minus=data_2(:,4);
plot(V_pot_sim,Vout_sim_plus,'b-');
hold on;
plot(V_pot_sim,Vout_sim_minus,'r-');
hold on;
plot(V_pot_exp,Vout_exp_plus,'g-');
hold on;
plot(V_pot_exp,Vout_exp_minus,'y-');
xlabel('potential divider voltage, V');
ylabel('output voltage, V');
legend('Vout + Sim','Vout - Sim','Vout + Exp','Vout - Exp');
title('Potential Divider verses Sensor Output Voltages ')