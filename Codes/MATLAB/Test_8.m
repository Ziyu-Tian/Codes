disp('Master Degree Classification');
a=input('Please enter the overall average:');
if(a<50)&&(a>=0)
    disp('failed');
elseif(a<60)&&(a>=50)
    disp('pass');
elseif (a<70)&&(a>=60)
    disp('Merit');
    elseif (a<=100)&&(a>=70)
    disp('Distinction');
    else 
    disp('The input number is invalid');
end 