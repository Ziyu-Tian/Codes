%Speed=data(11,4);
%Resistance=data(11,1);
%plot(Resistance,Speed,'d--')
%xlabel('Load Resistance(Ohm)');
%ylabel('Speed(rad/s)');
s=dsolve('Dy+2*y*sin(2x)=2*exp(cos(2x))','x');