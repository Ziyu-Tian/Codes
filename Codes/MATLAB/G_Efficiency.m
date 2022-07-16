%{
j=1;
k_t=63.6e-3;
for Resistance=0:1:1000
    g_voltage=data(j,5);
    g_current=data(j,6);
    m_current=data(j,3);
    v=data(j,4);
    Eff_G=g_current*g_voltage*100/((k_t*m_current-(14.32e-6)*v-7e-3)*v);
    plot(Resistance,Eff_G,'b*');
    hold all;
    j=j+1;
end
%}
Gen_Eff_2=data_2(:,5).*data_2(:,6)./(T_m_2.*data_2(:,4))*100;
Gen_Eff_4=data_4(:,5).*data_4(:,6)./(T_m_4.*data_4(:,4))*100;
plot(Resistance_2,Gen_Eff_2,'b-');
hold on;
plot(Resistance_4,Gen_Eff_4,'r-');
xlabel('load resistance,ohm');
ylabel('efficiency, %');
legend('Gen Effic Sim','Gen Effic Exp');
title('Generator Efficiency vs Load Resistance  ')

