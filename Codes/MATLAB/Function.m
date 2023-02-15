syms x;
f_1= atan(6./x)-atan(4./x);
f_2=rad2deg(f_1);
figure (1);
fplot(f_2,[0,100],'r');
grid on;
xlabel('x');
ylabel('y');
pause(200);


