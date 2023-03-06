syms t;
f_1= -2*cos(t)+2*sin(t);
%f_2=rad2deg(f_1);
figure (1);
fplot(f_1,[0,1000],'r');
grid on;
xlabel('t');
ylabel('x');
pause(200);


