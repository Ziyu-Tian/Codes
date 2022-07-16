#include<stdio.h>
int main()
{
    int a[10];
    float average;
    float sum=0;
    printf("Please input the numbers:");
    for (int i = 0; i < 10; i++)
    {
        scanf(" %d",&a[i]);
        sum+=a[i];
    }
    average=sum/10;
    printf("The sum is %.2f\n",sum);
    printf("The average is %.2f\n",average);
    return 0;
}
