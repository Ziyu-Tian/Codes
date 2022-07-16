%{
i=1;

    for Resistance_2=0:1:100 %1000
    m_current=data(i,3);
    m_voltage=data(i,2);
    g_voltage=data(i,5);
    g_current=data(i,6);
    P_i=m_voltage*m_current;
    P_o=g_current*g_voltage;
    plot(Resistance_2,P_i,'b*');
    hold all;
    plot(Resistance_2,P_o,'r*');
    hold all;
    i=i+1;
end
%}
P_in_2=data_2(:,2).*data_2(:,3);
P_out_2=data_2(:,5).*data_2(:,6);
P_in_4=data_4(:,2).*data_4(:,3);
P_out_4=data_4(:,5).*data_4(:,6);
Resistance_2=data_2(:,1);
Resistance_4=data_4(:,1);
plot(Resistance_2,P_in_2,'b-');
hold on;
plot(Resistance_2,P_out_2,'r-');
hold on;
plot(Resistance_4,P_in_4,'g-');
hold on;
plot(Resistance_4,P_out_4,'y-');
ylabel('power, W');
legend('P_{in} Sim','P_{out} Sim','P_{in} Exp','P_{out} Exp');
title('Input and Output Powers vs Load Resistance ')

