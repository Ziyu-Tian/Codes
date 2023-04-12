syms a;
syms b;
syms c;
syms L;
syms x;
f = -1/(25-c)^2*(x^2+x*(-2*c)+2*c*25-25^2);
area_3 = int(f, 'x', c, L);
Y = (1/L)*((b-a)/2+c-b+area_3);

firstDerivate = diff(Y, L);
%equation_b = diff(L, b);

%equation_c = diff(L, c);



S_2 = solve(firstDerivate ==0);

%S_3 = solve(equation_c);

X_1 = sprintf('L = %s\n', S_2);

%X_2 = sprintf('dL/db = %s\n', S_2);

%X_3 = sprintf('dL/dc = %s\n', S_3);

disp(S_2);


