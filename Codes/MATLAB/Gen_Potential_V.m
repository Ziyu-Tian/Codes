data_1=xlsread('Test_5.xlsx',1,'A2:D34');
data_2=xlsread('Test_6.xlsx',1,'A2:D34');
V_in=data_1(:,1);
V_pot_sim=data_1(:,2);
V_pot_exp=data_2(:,2);
plot(V_in,V_pot_exp,'r-');
hold on;
plot(V_in,V_pot_sim,'b-');
xlabel('generator voltage, V');
ylabel('potential divider voltage, V');
legend('Vpot Exp','Vpot Sim');
title('Generator versus Potential Divider Voltage')
