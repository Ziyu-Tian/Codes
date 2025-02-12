#include <stdio.h>
int main()
{
    int i=1,sum=0;
    int n;
    printf("Please input your number: ");
    scanf("%d",&n);
    do
    {
        while (i<=n)
        {
            sum=sum+i;
            i=i+1;
        }
        break;
    } while (n>0);
    printf("The sum is : %d",sum);
    
    return 0;
}