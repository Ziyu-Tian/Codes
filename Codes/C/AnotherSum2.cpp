#include <stdio.h>
float Fib(int n, int a);
int main()
{
    float sum=0;
    int n;
    scanf("%d", &n);
    for (int i = 1; i <=n; i++)
    {
        float temp;
        temp=Fib(i,2)/Fib(i,1);
        sum=sum+temp;
    }
    printf("%.2f",sum);
    return 0;
}
float Fib(int n, int a)
{
    if (n == 1)
    {
        return a;
    }
    else
    {
        if (n == 2)
        {
            return a + 1;
        }
        else
        {
            return Fib(n - 1, a) + Fib(n - 2, a);
        }
    }
}
