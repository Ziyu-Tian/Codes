k=1;
for Resistance=0:1:1000
    m_current=data(k,3);
    m_voltage=data(k,2);
    g_voltage=data(k,5);
    g_current=data(k,6);
    P_i=m_voltage*m_current;
    P_o=g_current*g_voltage;
    plot(Resistance,P_i,'bx');
    hold all;
    plot(Resistance,P_o,'rx');
    hold all;
    k=k+1;
end
xlabel('Resistance,ohm');
ylabel('Power, W');
legend('P_{in}','P_{out}')

