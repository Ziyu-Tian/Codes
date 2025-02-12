#include <stdio.h>
int PrimeCheck(int n);
int PrimeCheck(int n)
{
    int sum = 0;
    if (n == 2)
    {
        sum += 0;
    }
    else
    {
        for (int i = 2; i < n; i++)
        {
            if (n % i == 0)
            {
                sum += 1;
            }
        }
    }
    return sum;
}
int main()
{
    int n;
    scanf("%d", &n);
    for (int i = 2; i < n; i++)
    {
        if (PrimeCheck(i) == 0)
        {
            printf("%d\n",i);
        }
    }
    return 0;
}