syms x;
f_1=@(x)(x.^2-x+1)/(x-1);
figure (1);
fplot(f_1,[-50,50],'r');
grid on;
xlabel('x');
ylabel('y');
pause(200);


