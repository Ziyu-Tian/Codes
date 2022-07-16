num=xlsread('plasticdeformation.xlsx',1,'A2:C9');
strain=num(:,1);
Al=num(:,3);
y=@(x)69000*x;
plot(strain,Al,'bd');
hold on;
fplot(y,[0,0.01],'m');
grid on;
xlabel('strain');
ylabel('stress(MPa)');
legend('Al','Steel');
title('Stress-strain curve');
pause(20)


