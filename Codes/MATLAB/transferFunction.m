
den = poly([-1 -2 -3]);
figure(1);
g_1 = tf(1,den);
g_2 = tf(10,den);
g_3 = tf(100,den);
gcl = feedback(g_1, 1);
gcl_2 = feedback(g_2, 1);
gcl_3 = feedback(g_3, 1);
figure(1);
step(gcl);
hold on
step(gcl_2);
figure(2);
hold on
step(gcl_3);
%step(gcl,'-b');
%hold on
%step(gcl_2,'-r');
%hold on
%step(gcl_3,'-g');
grid on;

%{
figure(2);
step(1,[1 5 25]);
%damp(G_1)
grid on;
%}
%{
G_2 = tf(1,[1 -8 80]);
figure(2);
pzmap(G_2)
grid on;
G_3 = tf([1 4],[1 4 8 320]);
figure(3);
pzmap(G_3)
grid on;
G_4 = tf(1,[1 0 36]);
figure(4);
pzmap(G_4)
grid on;
%}
pause(1000);

