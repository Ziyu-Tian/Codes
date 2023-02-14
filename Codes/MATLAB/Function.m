syms x;
f_1=@(x)atan(6./x)-atan(4./x);
figure (1);
fplot(f_1,[0,100],'r');
grid on;
xlabel('x');
ylabel('y');
pause(200);


