#include <stdio.h>
#include<math.h>
#define e 2.7182838
int factorial(int n);
int factorial(int n)
{
    if (n > 1)
    {
        return n * factorial(n - 1);
    }
    else
    {
        return 1;
    }
}
int main()
{
    int n,x;
    int sum = 1;
    printf("Please input the n: ");
    scanf("%d", &n);
    printf("Please input the x: ");
    scanf("%d",&x);
    for (int i = 1; i <=n; i++)
    {
        sum =sum+ (pow(x,i)/factorial(i));
    }
    double series=exp(x);
    printf("The answer is %d\n", sum);
    printf("%.2f",series);
    return 0;
}