
g_1 = tf(1,[2 2 2]);
g_2 = tf(1, [1 6 11 6]);
g_3 = tf([1 2 2],[1 6 11 6]);
%step(gcl,'-b');
%hold on
%step(gcl_2,'-r');
%hold on
%step(gcl_3,'-g');
grid on;


figure(2);

rlocus(g_3);

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

