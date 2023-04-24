syms a;
syms b;
syms c;
syms d;
syms A;
syms B;
syms D;
A = -(2/3)*(1/(d-c)^2);
B = c/(d-c)^2;
D = (a+b)/2-c - (-1/(d-c)^2)*(2*c^2/3 - 2*c^2*d + d^2*c);

p = [A,B,0,D];
%equation_b = diff(L, b);

%equation_c = diff(L, c);



S_2 = roots(p);

%S_3 = solve(equation_c);


%X_2 = sprintf('dL/db = %s\n', S_2);

%X_3 = sprintf('dL/dc = %s\n', S_3);

disp(S_2);


