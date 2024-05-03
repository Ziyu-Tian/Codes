% 定义一个离散时间传递函数
num = [1 0.5 0.75];  % 分子多项式系数
den = [1 -0.6 -0.4 0.2 -1.4];  % 分母多项式系数
Ts = 1;  % 采样周期
sys = tf(num, den, Ts);  % 创建离散时间传递函数

% 获取极点
p = pole(sys);

% 检查稳定性
if all(abs(p) < 1)
    disp('The system is stable.');
else
    disp('The system is unstable.');
end
