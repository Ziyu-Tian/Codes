syms c;
f_1= -3/2*c+1;
f_2= -6*c+8;
f_3= -(27/2)*c+27;
%f_2=rad2deg(f_1);
figure (1);
fplot(f_1,[0,100],'r');
grid on;
hold on
fplot(f_2,[0,100],'b');
hold on
fplot(f_3,[0,1000],'k');
xlabel('c');
ylabel('y');
pause(200);


