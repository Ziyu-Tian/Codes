f = @(z,b,c) (z.^2-(25-c).*(5+b-2*c)+2*c*25-c.^2);
interval = [0 50 5 25 5 25];
fimplicit3(f,interval)
pause(1000);

