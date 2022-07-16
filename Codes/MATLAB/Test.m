Datum=xlsread('soup.xlsx',1,'A2:B11');
Time=Datum(:,1);
Temperature=Datum(:,2);
figure(2);
plot(Time,Temperature,'ko');
grid on;
xlabel('Time(s)');
ylabel('Temperature(degree)');
title('The soup curve');
pause(100);







