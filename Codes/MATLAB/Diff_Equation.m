syms t;
[x,y]=dsolve('Dx-2*x-y=0,Dy+18*x+7*y=0','x(0)=4','y(0)=-15',t);
disp([x,y]);