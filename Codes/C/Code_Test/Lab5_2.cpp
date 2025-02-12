#include <stdio.h>
#include<math.h>
long long int factorial(int n);// 'int' type is not enough for 20!
long long int factorial(int n)
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
    int n;
    long long int sum = 0;
    scanf("%d", &n);
    for (int i = 1; i <=n; i++)
    {
        sum += factorial(i);
    }
    printf("%lld", sum);
    return 0;
}