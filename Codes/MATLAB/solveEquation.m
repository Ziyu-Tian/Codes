syms a b c;
L = sqrt((25-c)*(a+b-2*c)+2*c*25-c^2);

equation_a = diff(L, a);

%equation_b = diff(L, b);

%equation_c = diff(L, c);

S_1 = solve(equation_a);

%S_2 = solve(equation_b);

%S_3 = solve(equation_c);

X_1 = sprintf('dL/da = %s\n', S_1);

%X_2 = sprintf('dL/db = %s\n', S_2);

%X_3 = sprintf('dL/dc = %s\n', S_3);

disp(X_1);
