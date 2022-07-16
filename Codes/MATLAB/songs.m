%{
c=imread('c.jpg');
c=rgb2gray(c);
[m,n]=size(c);
p=edge(c,'sobel');
figure(2);
imshow(p);
pause(100);
%}
Fs = 8192;
x1 = [5 5 6 5 8 7 ];
x2 = [0.5 0.5 1 1 1 2 ];
for i = 1:6
    y = gen_wave(x1(i),x2(i));
    sound(y, Fs);
    n = 0.5;
    pause(n);

end
pause(1);
x1 = [5 5 6 5 9 8 ];
x2 = [0.5 0.5 1 1 1 2 ];
for i = 1:6
    y = gen_wave(x1(i),x2(i));
    sound(y, Fs);
    n = 0.5;
    pause(n);

end
pause(0.5);
x1 = [ 5 5 12 10 8 7 6 ];
x2 = [ 0.5 0.5 1 1 1 1 2.5 ];
for i = 1:6
    y = gen_wave(x1(i),x2(i));
    sound(y, Fs);
    n = 0.5;
    pause(n);

end
pause(0.5);
x1=[11 11 10 8 9 8 ];
x2=[0.5 0.5 1 1 1 1 ];
for i = 1:6
    y= gen_wave(x1(i),x2(i));
    sound(y,Fs);
    n=0.5;
    pause(n);
end


 
 