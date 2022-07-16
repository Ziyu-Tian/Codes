%{
j=1;
for Resistance=0:1:1000
    m_current=data(j,3);
    m_voltage=data(j,2);
    g_voltage=data(j,5);
    g_current=data(j,6);
    P_i=m_voltage*m_current;
    P_o=g_current*g_voltage;
    Efficiency=P_o/P_i*100;
    plot(Resistance,Efficiency,'b*');
    hold all;
    j=j+1;
end
%}
Eff_1=(P_out_1./P_in_1)*100;
Eff_3=(P_out_3./P_in_3)*100;
plot(Resistance_1,Eff_1,'b-');
hold on;
plot(Resistance_3,Eff_3,'r-');
xlabel('load resistance,ohm');
ylabel('efficiency, %');
legend('Sim Effic','Exp Effic');
title('Overall System Efficiency vs Load Resistance ')
