syms z;

% ����һ������ʽ���ʽ
poly_expr = (z+1)^4;

% ʹ�� expand ����չ������ʽ
expanded_poly = expand(poly_expr);

% ��ʾչ����Ľ��
disp('ԭʼ����ʽ���ʽ:');
disp(poly_expr);
disp('չ����Ķ���ʽ���ʽ:');
disp(expanded_poly);
