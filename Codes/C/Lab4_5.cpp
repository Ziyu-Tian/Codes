#include <stdio.h>
int main()
{
    int x,n;
    int power=1;
    int i=1;
    printf("Please input the root and power:");
    scanf("%d %d",&x,&n);
    while (i<=n)
    {
        power=power*x;
        i++;
    }
    printf("The power is %d",power);
    return 0;
}