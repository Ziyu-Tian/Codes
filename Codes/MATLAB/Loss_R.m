%{
i=1;
for Resistance=0:1:1000
    g_voltage=data(i,5);
    g_current=data(i,6);
    m_current=data(i,3);
    m_voltage=data(i,2);
    v=data(i,4);
    m_loss=m_current*m_voltage-k_t*m_current*v;
    g_loss=(k_t*m_current-14.32e-6*v-7e-3)*v-g_current*g_voltage; 
    plot(Resistance,m_loss,'bx');
    hold all;
    plot(Resistance,g_loss,'rx');
    hold all;
    i=i+1;
end
%}
%{
m_loss_2=data_2(:,3).*data_2(:,2)-T_e_2.*data_2(:,4);
g_loss_2=T_m_2.*data_2(:,4)-data_2(:,6).*data_2(:,5);
m_loss_4=data_4(:,3).*data_4(:,2)-T_e_4.*data_4(:,4);
g_loss_4=T_m_4.*data_4(:,4)-data_4(:,6).*data_4(:,5);
plot(Resistance_2,m_loss_2,'b-');
hold on;
plot(Resistance_2,g_loss_2,'r-');
hold on;
plot(Resistance_4,m_loss_4,'g-');
hold on;
plot(Resistance_4,g_loss_4,'y');
%}
P_ex_2=P_in_2-1.4*data_2(:,3).^2-2*B*data_2(:,4).^2-2*T_sf*data_2(:,4)-1.4*data_2(:,6).^2-data_2(:,6).^2.*Resistance_2;
P_ex_4=P_in_4-1.4*data_4(:,3).^2-2*B*data_4(:,4).^2-2*T_sf*data_4(:,4)-1.4*data_4(:,6).^2-data_4(:,6).^2.*Resistance_4;
plot(Resistance_2,P_ex_2,'r-');
hold on;
plot(Resistance_4,P_ex_4,'b-');
%}
xlabel('load resistance, ohm');
ylabel('power, W');
legend('P_{extra} Sim','P_{extra} Exp');
title('Extral Losses vs Load Resistance ')
