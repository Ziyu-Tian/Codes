t=[1:10];
y=3*sin(t)./t-1;
plot(t,y,'rd');
hold on;
yy=@(t)3*sin(t)./t-1;
fplot(yy,[1,10],'b-');
legend('calculated','function');
xlabel('Times(s)');
ylabel('Displacement');
fzero(yy,2)


