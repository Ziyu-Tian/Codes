G_1 = tf(1,[1 4]);
figure(1);
pzmap(G_1)
grid on;
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
pause(1000);

