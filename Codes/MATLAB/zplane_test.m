% ����һ����ɢʱ�䴫�ݺ���
num = [1 0.5 0.75];  % ���Ӷ���ʽϵ��
den = [1 -0.6 -0.4 0.2 -1.4];  % ��ĸ����ʽϵ��
Ts = 1;  % ��������
sys = tf(num, den, Ts);  % ������ɢʱ�䴫�ݺ���

% ��ȡ����
p = pole(sys);

% ����ȶ���
if all(abs(p) < 1)
    disp('The system is stable.');
else
    disp('The system is unstable.');
end
