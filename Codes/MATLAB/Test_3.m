t=[1:10];
dist=[24.1,60.5,109,170,242.1,320.8,422.6,530.2,648,782];
p=polyfit(t,dist,2);
%dist2=p(1)*t.^2+p(2)*t+p(3);%Method_1;
%plot(t,dist,'ro');
%hold on;
%plot(t,dist2,'b-');%Method_1;
%dist3=@(x)polyval(p,x);%Method_2;
%figure(2);
%plot(t,dist,'ro');
%hold;
%fplot(dist3,[1,10],'b-');%Method_2;
dist3=@(x)polyval(p,x);
figure(3);
plot(t,dist,'ro');
hold;
plot(t,dist3(t),'b-');
xlabel('Time(s)');
ylabel('Distance(m)');
legend('Measured Data','Fitted');
title('x-t curve')


