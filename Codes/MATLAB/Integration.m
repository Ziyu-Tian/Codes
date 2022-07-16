syms x;
%f=@(x,y)7+4.*x.*y;
y=(8*x+4)^(8/9);
result=int(y,x,2,4);
%result = integral2(f, 0, 3, 1, 3);
disp(result);
%fprintf('%f',result);