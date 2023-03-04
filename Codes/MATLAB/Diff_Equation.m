syms t;
fun = (s+1)*(s+2)*(s^2+20*s+1700);
s=solve(fun,s);
disp([x,y]);