#include<stdio.h>
int main()
{
    int num;
    int count=0;
    printf("Please input your number: ");
    scanf("%d",&num);
    while(num!=0)
    {
        num=num/10;
        count++;
    }
    printf("Number of digits is %d",count);
    return 0;
}