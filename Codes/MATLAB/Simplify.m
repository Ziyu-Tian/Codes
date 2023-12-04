syms z;

% 定义一个多项式表达式
poly_expr = (z+1)^4;

% 使用 expand 函数展开多项式
expanded_poly = expand(poly_expr);

% 显示展开后的结果
disp('原始多项式表达式:');
disp(poly_expr);
disp('展开后的多项式表达式:');
disp(expanded_poly);
