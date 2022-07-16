a1=input('The answer to the Question 1 is : (correct:1, incorrect: 0)');
a2=input('The answer to the Question 2 is :(correct:1,incorrect:0)');
if (a1==0) && (a2==0)
    disp('Nothing this time.  Try it next time');
    elseif (a1==0) && (a2==1)
    disp('Congratulations! You will get one fortune cookie');
    elseif (a1==1) && (a2==0)
    disp('Congratulations! You will get one fortune cookie');
    elseif (a1==1) && (a2==1)
    disp('Wow! You will get two fortune cookies');
end