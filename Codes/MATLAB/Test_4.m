%f1=@(x)x.^2+4*sin(x)-16;
%fplot(f1,[-6,6]);
%grid on;
%x1=fzero(f1,-4);
%x2=fzero(f1,4)
y=@(x)x.^3-2*(x.^2)-x+2;
fplot(y,[-3,3]);
grid on;
x1=fzero(y,-1);
x2=fzero(y,1);
x3=fzero(y,2)